// noiseutils.cpp
//
// Copyright (C) 2003-2005 Jason Bevins
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// The developer's email is jlbezigvins@gmzigail.com (for great email, take
// off every 'zig').
//
// Updated for C++17 compatibility by TEK Nemesis and Grok on April 22, 2025.
// - Updated WriterBMP::WriteDestFile and WriterTER::WriteDestFile to handle
//   std::filesystem::path for cross-platform file operations.
// - Added definitions for Image class methods (constructors, destructor,
//   operator=, Clear, GetValue, ReclaimMem, SetSize, SetValue, TakeOwnership)
//   to resolve linker warnings (VC7001), matching the original implementations
//   in noiseutils-orig.cpp.
// - Added detailed comments to Image class methods for better code documentation,
//   including confidence notes to align with NoiseMap's behavior.
// - Updated Image class to use std::vector instead of raw pointers, fixing memory
//   management issues and correcting memcpy size errors in the original.
// - Fixed potential integer overflow in BlendChannel using std::clamp.
// - Added multi-threading to NoiseMapBuilderSphere::Build() on April 23, 2025,
//   to parallelize noise map generation across rows, aiming to improve performance
//   on multi-core systems. Note: Significant speedup observed in Release mode
//   (from ~30s to ~9s), but Debug mode showed no improvement due to lack of
//   compiler optimizations and thread overhead.

#include "noiseutils/noiseutils.h"

#include <fstream>
#include <algorithm>
#include <thread>
#include <vector>

#include <noise/interp.h>
#include <noise/mathconsts.h>
#include <noise/model/cylinder.h>
#include <noise/model/plane.h>
#include <noise/model/sphere.h>

// Bitmap header size.
constexpr int BMP_HEADER_SIZE = 54;

// Direction of the light source, in compass degrees (0 = north, 90 = east,
// 180 = south, 270 = east)
constexpr double DEFAULT_LIGHT_AZIMUTH = 45.0;

// Amount of contrast between light and dark areas.
constexpr double DEFAULT_LIGHT_CONTRAST = 1.0;

// Elevation of the light source above the horizon, in degrees (0 = on
// horizon, 90 = directly overhead)
constexpr double DEFAULT_LIGHT_ELEVATION = 45.0;

namespace noise {

    namespace utils {

        // Constructor for Image with specified dimensions, sets the size of the image.
        // Confidence: High - Matches NoiseMap's constructor, which calls SetSize to initialize the buffer.
        Image::Image(int width, int height) {
            SetSize(width, height);
        }

        // Copy constructor for Image, creates a deep copy of the source image.
        // Confidence: High - Matches NoiseMap's copy constructor, performing a deep copy of all members.
        Image::Image(const Image& rhs) {
            m_borderValue = rhs.m_borderValue;
            m_height = rhs.m_height;
            m_width = rhs.m_width;
            m_stride = rhs.m_stride;
            m_values = rhs.m_values;
        }

        // Assignment operator for Image, performs a deep copy of the source image.
        // Confidence: High - Matches NoiseMap's assignment operator, ensuring a deep copy.
        Image& Image::operator=(const Image& rhs) {
            m_borderValue = rhs.m_borderValue;
            m_height = rhs.m_height;
            m_width = rhs.m_width;
            m_stride = rhs.m_stride;
            m_values = rhs.m_values;
            return *this;
        }

        // Clears the image by setting all pixels to the specified color.
        // Confidence: High - Matches NoiseMap::Clear, using std::fill to set all elements.
        void Image::Clear(const Color& value) {
            std::fill(m_values.begin(), m_values.end(), value);
        }

        // Retrieves the color value at the specified (x, y) position in the image.
        // Returns the border value if the position is outside the image bounds.
        // Confidence: High - Matches NoiseMap::GetValue, with identical bounds checking logic.
        Color Image::GetValue(int x, int y) const noexcept {
            if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
                return m_values[y * m_stride + x];
            }
            return m_borderValue;
        }

        // Reallocates the image buffer to minimize memory usage by removing stride padding.
        // Confidence: High - Matches NoiseMap::ReclaimMem, with identical logic adjusted for Color type.
        void Image::ReclaimMem() {
            std::vector<Color> newValues(m_width * m_height);
            for (int y = 0; y < m_height; ++y) {
                for (int x = 0; x < m_width; ++x) {
                    newValues[y * m_width + x] = m_values[y * m_stride + x];
                }
            }
            m_values = std::move(newValues);
            m_stride = m_width;
        }

        // Sets the dimensions of the image, resizing the internal buffer.
        // Throws ExceptionInvalidParam if dimensions are invalid.
        // Confidence: High - Matches NoiseMap::SetSize, with identical validation and resizing logic.
        void Image::SetSize(int width, int height) {
            if (width < 0 || height < 0
                || width > RASTER_MAX_WIDTH || height > RASTER_MAX_HEIGHT) {
                throw noise::ExceptionInvalidParam();
            }
            if (width == 0 || height == 0) {
                m_values.clear();
                m_width = 0;
                m_height = 0;
                m_stride = 0;
            } else {
                m_values.resize(static_cast<size_t>(width) * height);
                m_width = width;
                m_height = height;
                m_stride = CalcStride(width);
            }
        }

        // Sets the color value at the specified (x, y) position in the image.
        // Does nothing if the position is outside the image bounds.
        // Confidence: High - Matches NoiseMap::SetValue, with identical bounds checking.
        void Image::SetValue(int x, int y, const Color& value) noexcept {
            if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
                m_values[y * m_stride + x] = value;
            }
        }

        // Transfers ownership of the source image's buffer to this image, clearing the source.
        void Image::TakeOwnership(Image& source) noexcept {
            m_borderValue = source.m_borderValue;
            m_height = source.m_height;
            m_width = source.m_width;
            m_stride = source.m_stride;
            m_values = std::move(source.m_values);

            source.m_borderValue = Color(0, 0, 0, 0);
            source.m_height = 0;
            source.m_width = 0;
            source.m_stride = 0;
            source.m_values.clear();
        }

        // Performs linear interpolation between two 8-bit channel values.
        [[nodiscard]] inline uint8 BlendChannel(uint8 channel0, uint8 channel1, float alpha) noexcept {
            float c0 = static_cast<float>(channel0) / 255.0f;
            float c1 = static_cast<float>(channel1) / 255.0f;
            float result = (c1 * alpha) + (c0 * (1.0f - alpha));
            return static_cast<uint8>(std::clamp(result * 255.0f, 0.0f, 255.0f));
        }

        // Performs linear interpolation between two colors and stores the result
        // in out.
        inline void LinearInterpColor(const Color& color0, const Color& color1, float alpha, Color& out) noexcept {
            out.alpha = BlendChannel(color0.alpha, color1.alpha, alpha);
            out.blue = BlendChannel(color0.blue, color1.blue, alpha);
            out.green = BlendChannel(color0.green, color1.green, alpha);
            out.red = BlendChannel(color0.red, color1.red, alpha);
        }

        //////////////////////////////////////////////////////////////////////////////
        // GradientColor class

        GradientColor::GradientColor() noexcept {
        }

        void GradientColor::AddGradientPoint(double gradientPos,
            const Color& gradientColor) {
            // Find the insertion point for the new gradient point and insert the new
            // gradient point at that insertion point.  The gradient point array will
            // remain sorted by gradient position.
            int insertionPos = FindInsertionPos(gradientPos);
            InsertAtPos(insertionPos, gradientPos, gradientColor);
        }

        void GradientColor::Clear() noexcept {
            m_pGradientPoints.reset();
            m_gradientPointCount = 0;
        }

        int GradientColor::FindInsertionPos(double gradientPos) {
            int insertionPos;
            for (insertionPos = 0; insertionPos < m_gradientPointCount;
                insertionPos++) {
                if (gradientPos < m_pGradientPoints[insertionPos].pos) {
                    // We found the array index in which to insert the new gradient point.
                    // Exit now.
                    break;
                } else if (gradientPos == m_pGradientPoints[insertionPos].pos) {
                    // Each gradient point is required to contain a unique gradient
                    // position, so throw an exception.
                    throw noise::ExceptionInvalidParam();
                }
            }
            return insertionPos;
        }

        const Color& GradientColor::GetColor(double gradientPos) const {
            assert(m_gradientPointCount >= 2);

            // Find the first element in the gradient point array that has a gradient
            // position larger than the gradient position passed to this method.
            int indexPos;
            for (indexPos = 0; indexPos < m_gradientPointCount; indexPos++) {
                if (gradientPos < m_pGradientPoints[indexPos].pos) {
                    break;
                }
            }

            // Find the two nearest gradient points so that we can perform linear
            // interpolation on the color.
            int index0 = ClampValue(indexPos - 1, 0, m_gradientPointCount - 1);
            int index1 = ClampValue(indexPos, 0, m_gradientPointCount - 1);

            // If some gradient points are missing (which occurs if the gradient
            // position passed to this method is greater than the largest gradient
            // position or less than the smallest gradient position in the array), get
            // the corresponding gradient color of the nearest gradient point and exit
            // now.
            if (index0 == index1) {
                m_workingColor = m_pGradientPoints[index1].color;
                return m_workingColor;
            }

            // Compute the alpha value used for linear interpolation.
            double input0 = m_pGradientPoints[index0].pos;
            double input1 = m_pGradientPoints[index1].pos;
            double alpha = (gradientPos - input0) / (input1 - input0);

            // Now perform the linear interpolation given the alpha value.
            const Color& color0 = m_pGradientPoints[index0].color;
            const Color& color1 = m_pGradientPoints[index1].color;
            LinearInterpColor(color0, color1, static_cast<float>(alpha), m_workingColor);
            return m_workingColor;
        }

        void GradientColor::InsertAtPos(int insertionPos, double gradientPos,
            const Color& gradientColor) {
            // Make room for the new gradient point at the specified insertion position
            // within the gradient point array.  The insertion position is determined by
            // the gradient point's position; the gradient points must be sorted by
            // gradient position within that array.
            auto newGradientPoints = std::make_unique<GradientPoint[]>(m_gradientPointCount + 1);
            for (int i = 0; i < m_gradientPointCount; i++) {
                if (i < insertionPos) {
                    newGradientPoints[i] = m_pGradientPoints[i];
                } else {
                    newGradientPoints[i + 1] = m_pGradientPoints[i];
                }
            }
            m_pGradientPoints = std::move(newGradientPoints);
            ++m_gradientPointCount;

            // Now that we've made room for the new gradient point within the array, add
            // the new gradient point.
            m_pGradientPoints[insertionPos].pos = gradientPos;
            m_pGradientPoints[insertionPos].color = gradientColor;
        }

        //////////////////////////////////////////////////////////////////////////////
        // NoiseMap class

        NoiseMap::NoiseMap() noexcept {
        }

        NoiseMap::NoiseMap(int width, int height) {
            SetSize(width, height);
        }

        NoiseMap::NoiseMap(const NoiseMap& rhs) {
            m_borderValue = rhs.m_borderValue;
            m_height = rhs.m_height;
            m_width = rhs.m_width;
            m_stride = rhs.m_stride;
            m_values = rhs.m_values;
        }

        NoiseMap::~NoiseMap() noexcept {
        }

        NoiseMap& NoiseMap::operator=(const NoiseMap& rhs) {
            m_borderValue = rhs.m_borderValue;
            m_height = rhs.m_height;
            m_width = rhs.m_width;
            m_stride = rhs.m_stride;
            m_values = rhs.m_values;
            return *this;
        }

        void NoiseMap::Clear(float value) noexcept {
            std::fill(m_values.begin(), m_values.end(), value);
        }

        float NoiseMap::GetValue(int x, int y) const noexcept {
            if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
                return m_values[y * m_stride + x];
            }
            // The coordinates specified are outside the noise map.  Return the border
            // value.
            return m_borderValue;
        }

        void NoiseMap::ReclaimMem() {
            std::vector<float> newValues(m_width * m_height);
            for (int y = 0; y < m_height; ++y) {
                for (int x = 0; x < m_width; ++x) {
                    newValues[y * m_width + x] = m_values[y * m_stride + x];
                }
            }
            m_values = std::move(newValues);
            m_stride = m_width;
        }

        void NoiseMap::SetSize(int width, int height) {
            if (width < 0 || height < 0
                || width > RASTER_MAX_WIDTH || height > RASTER_MAX_HEIGHT) {
                // Invalid width or height.
                throw noise::ExceptionInvalidParam();
            }
            if (width == 0 || height == 0) {
                // An empty noise map was specified.
                m_values.clear();
                m_width = 0;
                m_height = 0;
                m_stride = 0;
            } else {
                // A new noise map size was specified.
                m_values.resize(static_cast<size_t>(width) * height);
                m_width = width;
                m_height = height;
                m_stride = CalcStride(width);
            }
        }

        void NoiseMap::SetValue(int x, int y, float value) noexcept {
            if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
                m_values[y * m_stride + x] = value;
            }
        }

        void NoiseMap::TakeOwnership(NoiseMap& source) noexcept {
            // Move the values and the noise map buffer from the source noise map to
            // this noise map.
            m_borderValue = source.m_borderValue;
            m_height = source.m_height;
            m_width = source.m_width;
            m_stride = source.m_stride;
            m_values = std::move(source.m_values);

            // Now that the source buffer is assigned to this noise map, reset the
            // source noise map object.
            source.m_borderValue = 0.0f;
            source.m_height = 0;
            source.m_width = 0;
            source.m_stride = 0;
            source.m_values.clear();
        }

        //////////////////////////////////////////////////////////////////////////////
        // WriterBMP class

        int WriterBMP::CalcWidthByteCount(int width) const noexcept {
            return ((width * 3) + 3) & ~0x03;
        }

        void WriterBMP::WriteDestFile() {
            if (!m_pSourceImage) {
                throw noise::ExceptionInvalidParam();
            }

            int width = m_pSourceImage->GetWidth();
            int height = m_pSourceImage->GetHeight();

            int bufferSize = CalcWidthByteCount(width);
            int destSize = bufferSize * height;

            auto pLineBuffer = std::make_unique<uint8[]>(bufferSize);

            std::ofstream os(m_destFilename.string(), std::ios::out | std::ios::binary);
            if (!os) {
                throw noise::ExceptionUnknown();
            }

            uint8 d[4];
            os.write("BM", 2);
            auto* p1 = UnpackLittle32(d, destSize + BMP_HEADER_SIZE);
            os.write(reinterpret_cast<char*>(p1), 4);
            os.write("\0\0\0\0", 4);
            auto* p2 = UnpackLittle32(d, static_cast<uint32>(BMP_HEADER_SIZE));
            os.write(reinterpret_cast<char*>(p2), 4);
            auto* p3 = UnpackLittle32(d, 40);
            os.write(reinterpret_cast<char*>(p3), 4);
            auto* p4 = UnpackLittle32(d, static_cast<uint32>(width));
            os.write(reinterpret_cast<char*>(p4), 4);
            auto* p5 = UnpackLittle32(d, static_cast<uint32>(height));
            os.write(reinterpret_cast<char*>(p5), 4);
            auto* p6 = UnpackLittle16(d, 1);
            os.write(reinterpret_cast<char*>(p6), 2);
            auto* p7 = UnpackLittle16(d, 24);
            os.write(reinterpret_cast<char*>(p7), 2);
            os.write("\0\0\0\0", 4);
            auto* p8 = UnpackLittle32(d, static_cast<uint32>(destSize));
            os.write(reinterpret_cast<char*>(p8), 4);
            auto* p9 = UnpackLittle32(d, 2834);
            os.write(reinterpret_cast<char*>(p9), 4);
            auto* p10 = UnpackLittle32(d, 2834);
            os.write(reinterpret_cast<char*>(p10), 4);
            os.write("\0\0\0\0", 4);
            os.write("\0\0\0\0", 4);
            if (!os) {
                throw noise::ExceptionUnknown();
            }

            for (int y = 0; y < height; y++) {
                std::fill(pLineBuffer.get(), pLineBuffer.get() + bufferSize, 0);
                const Color* pSource = m_pSourceImage->GetConstSlabPtr(0, y);
                uint8* pDest = pLineBuffer.get();
                for (int x = 0; x < width; x++) {
                    pDest[0] = pSource->blue;
                    pDest[1] = pSource->green;
                    pDest[2] = pSource->red;
                    pDest += 3;
                    ++pSource;
                }
                os.write(reinterpret_cast<char*>(pLineBuffer.get()), bufferSize);
                if (!os) {
                    throw noise::ExceptionUnknown();
                }
            }
        }

        //////////////////////////////////////////////////////////////////////////////
        // WriterTER class

        int WriterTER::CalcWidthByteCount(int width) const noexcept {
            return (width * sizeof(int16));
        }

        void WriterTER::WriteDestFile() {
            if (!m_pSourceNoiseMap) {
                throw noise::ExceptionInvalidParam();
            }

            int width = m_pSourceNoiseMap->GetWidth();
            int height = m_pSourceNoiseMap->GetHeight();

            int bufferSize = CalcWidthByteCount(width);
            int destSize = bufferSize * height;

            auto pLineBuffer = std::make_unique<uint8[]>(bufferSize);

            std::ofstream os(m_destFilename.string(), std::ios::out | std::ios::binary);
            if (!os) {
                throw noise::ExceptionUnknown();
            }

            uint8 d[4];
            int16 heightScale = static_cast<int16>(std::floor(32768.0 / static_cast<double>(m_metersPerPoint)));
            os.write("TERRAGENTERRAIN ", 16);
            os.write("SIZE", 4);
            auto* p1 = UnpackLittle16(d, GetMin(width, height) - 1);
            os.write(reinterpret_cast<char*>(p1), 2);
            os.write("\0\0", 2);
            os.write("XPTS", 4);
            auto* p2 = UnpackLittle16(d, width);
            os.write(reinterpret_cast<char*>(p2), 2);
            os.write("\0\0", 2);
            os.write("YPTS", 4);
            auto* p3 = UnpackLittle16(d, height);
            os.write(reinterpret_cast<char*>(p3), 2);
            os.write("\0\0", 2);
            os.write("SCAL", 4);
            auto* p4 = UnpackFloat(d, m_metersPerPoint);
            os.write(reinterpret_cast<char*>(p4), 4);
            auto* p5 = UnpackFloat(d, m_metersPerPoint);
            os.write(reinterpret_cast<char*>(p5), 4);
            auto* p6 = UnpackFloat(d, m_metersPerPoint);
            os.write(reinterpret_cast<char*>(p6), 4);
            os.write("ALTW", 4);
            auto* p7 = UnpackLittle16(d, heightScale);
            os.write(reinterpret_cast<char*>(p7), 2);
            os.write("\0\0", 2);
            if (!os) {
                throw noise::ExceptionUnknown();
            }

            for (int y = 0; y < height; y++) {
                const float* pSource = m_pSourceNoiseMap->GetConstSlabPtr(0, y);
                uint8* pDest = pLineBuffer.get();
                for (int x = 0; x < width; x++) {
                    int16 scaledHeight = static_cast<int16>(std::floor(*pSource * 2.0));
                    auto* pDestUpdated = UnpackLittle16(pDest, scaledHeight);
                    pDest = pDestUpdated;
                    pDest += 2;
                    ++pSource;
                }
                os.write(reinterpret_cast<char*>(pLineBuffer.get()), bufferSize);
                if (!os) {
                    throw noise::ExceptionUnknown();
                }
            }
        }

        //////////////////////////////////////////////////////////////////////////////
        // NoiseMapBuilder class

        NoiseMapBuilder::NoiseMapBuilder() noexcept :
            m_pCallback(nullptr),
            m_destHeight(0),
            m_destWidth(0),
            m_pDestNoiseMap(nullptr) {
        }

        void NoiseMapBuilder::SetDestSize(int destWidth, int destHeight) {
            if (destWidth < 0 || destHeight < 0) {
                throw noise::ExceptionInvalidParam();
            }
            m_destWidth = destWidth;
            m_destHeight = destHeight;
        }

        void NoiseMapBuilder::SetCallback(NoiseMapCallback pCallback) {
            m_pCallback = pCallback;
        }

        //////////////////////////////////////////////////////////////////////////////
        // NoiseMapBuilderCylinder class

        NoiseMapBuilderCylinder::NoiseMapBuilderCylinder() noexcept :
            m_lowerAngleBound(0.0),
            m_lowerHeightBound(0.0),
            m_upperAngleBound(0.0),
            m_upperHeightBound(0.0) {
        }

        void NoiseMapBuilderCylinder::Build() {
            if (m_upperAngleBound <= m_lowerAngleBound
                || m_upperHeightBound <= m_lowerHeightBound
                || m_destWidth <= 0
                || m_destHeight <= 0
                || !m_sourceModules
                || !m_pDestNoiseMap) {
                throw noise::ExceptionInvalidParam();
            }

            // Resize the destination noise map so that it can store the new output
            // values from the source model.
            m_pDestNoiseMap->SetSize(m_destWidth, m_destHeight);

            // Create the cylinder model.
            model::Cylinder cylinderModel(*m_sourceModules);

            double angleExtent = m_upperAngleBound - m_lowerAngleBound;
            double heightExtent = m_upperHeightBound - m_lowerHeightBound;
            double xDelta = angleExtent / static_cast<double>(m_destWidth);
            double yDelta = heightExtent / static_cast<double>(m_destHeight);
            double curAngle = m_lowerAngleBound;
            double curHeight = m_lowerHeightBound;

            // Fill every point in the noise map with the output values from the model.
            for (int y = 0; y < m_destHeight; y++) {
                float* pDest = m_pDestNoiseMap->GetSlabPtr(0, y);
                curAngle = m_lowerAngleBound;
                for (int x = 0; x < m_destWidth; x++) {
                    float curValue = static_cast<float>(cylinderModel.GetValue(curAngle, curHeight));
                    *pDest++ = curValue;
                    curAngle += xDelta;
                }
                curHeight += yDelta;
                if (m_pCallback) {
                    m_pCallback(y);
                }
            }
        }

        void NoiseMapBuilderCylinder::SetBounds(double lowerAngleBound, double upperAngleBound,
            double lowerHeightBound, double upperHeightBound) {
            if (lowerAngleBound >= upperAngleBound || lowerHeightBound >= upperHeightBound) {
                throw noise::ExceptionInvalidParam();
            }

            m_lowerAngleBound = lowerAngleBound;
            m_upperAngleBound = upperAngleBound;
            m_lowerHeightBound = lowerHeightBound;
            m_upperHeightBound = upperHeightBound;
        }

        //////////////////////////////////////////////////////////////////////////////
        // NoiseMapBuilderPlane class

        NoiseMapBuilderPlane::NoiseMapBuilderPlane() noexcept :
            m_isSeamlessEnabled(false),
            m_lowerXBound(0.0),
            m_lowerZBound(0.0),
            m_upperXBound(0.0),
            m_upperZBound(0.0) {
        }

        void NoiseMapBuilderPlane::Build() {
            if (m_upperXBound <= m_lowerXBound
                || m_upperZBound <= m_lowerZBound
                || m_destWidth <= 0
                || m_destHeight <= 0
                || !m_sourceModules
                || !m_pDestNoiseMap) {
                throw noise::ExceptionInvalidParam();
            }

            // Resize the destination noise map so that it can store the new output
            // values from the source model.
            m_pDestNoiseMap->SetSize(m_destWidth, m_destHeight);

            // Create the plane model.
            model::Plane planeModel(*m_sourceModules);

            double xExtent = m_upperXBound - m_lowerXBound;
            double zExtent = m_upperZBound - m_lowerZBound;
            double xDelta = xExtent / static_cast<double>(m_destWidth);
            double zDelta = zExtent / static_cast<double>(m_destHeight);
            double xCur = m_lowerXBound;
            double zCur = m_lowerZBound;

            // Fill every point in the noise map with the output values from the model.
            for (int z = 0; z < m_destHeight; z++) {
                float* pDest = m_pDestNoiseMap->GetSlabPtr(0, z);
                xCur = m_lowerXBound;
                for (int x = 0; x < m_destWidth; x++) {
                    float finalValue;
                    if (!m_isSeamlessEnabled) {
                        finalValue = planeModel.GetValue(xCur, zCur);
                    } else {
                        double swValue = planeModel.GetValue(xCur, zCur);
                        double seValue = planeModel.GetValue(xCur + xExtent, zCur);
                        double nwValue = planeModel.GetValue(xCur, zCur + zExtent);
                        double neValue = planeModel.GetValue(xCur + xExtent, zCur + zExtent);
                        double xBlend = 1.0 - ((xCur - m_lowerXBound) / xExtent);
                        double zBlend = 1.0 - ((zCur - m_lowerZBound) / zExtent);
                        double z0 = LinearInterp(swValue, seValue, xBlend);
                        double z1 = LinearInterp(nwValue, neValue, xBlend);
                        finalValue = static_cast<float>(LinearInterp(z0, z1, zBlend));
                    }
                    *pDest++ = finalValue;
                    xCur += xDelta;
                }
                zCur += zDelta;
                if (m_pCallback) {
                    m_pCallback(z);
                }
            }
        }

        void NoiseMapBuilderPlane::SetBounds(double lowerXBound, double upperXBound,
            double lowerZBound, double upperZBound) {
            if (lowerXBound >= upperXBound || lowerZBound >= upperZBound) {
                throw noise::ExceptionInvalidParam();
            }

            m_lowerXBound = lowerXBound;
            m_upperXBound = upperXBound;
            m_lowerZBound = lowerZBound;
            m_upperZBound = upperZBound;
        }

        //////////////////////////////////////////////////////////////////////////////
        // NoiseMapBuilderSphere class

        NoiseMapBuilderSphere::NoiseMapBuilderSphere() noexcept :
            m_eastLonBound(0.0),
            m_northLatBound(0.0),
            m_southLatBound(0.0),
            m_westLonBound(0.0) {
        }

        void NoiseMapBuilderSphere::Build() {
            if (m_eastLonBound <= m_westLonBound
                || m_northLatBound <= m_southLatBound
                || m_destWidth <= 0
                || m_destHeight <= 0
                || !m_sourceModules
                || !m_pDestNoiseMap) {
                throw noise::ExceptionInvalidParam();
            }

            // Resize the destination noise map so that it can store the new output
            // values from the source model.
            m_pDestNoiseMap->SetSize(m_destWidth, m_destHeight);

            // Create the sphere model.
            model::Sphere sphereModel(*m_sourceModules);

            double lonExtent = m_eastLonBound - m_westLonBound;
            double latExtent = m_northLatBound - m_southLatBound;
            double xDelta = lonExtent / static_cast<double>(m_destWidth);
            double yDelta = latExtent / static_cast<double>(m_destHeight);

            // Determine the number of threads to use (use hardware concurrency)
            unsigned int numThreads = std::thread::hardware_concurrency();
            if (numThreads == 0) numThreads = 4; // Fallback to 4 threads if hardware concurrency is unavailable
            numThreads = std::min(numThreads, static_cast<unsigned int>(m_destHeight)); // Don't use more threads than rows

            // Divide rows among threads
            std::vector<std::thread> threads;
            int rowsPerThread = m_destHeight / numThreads;
            int remainingRows = m_destHeight % numThreads;
            int startRow = 0;

            for (unsigned int t = 0; t < numThreads; ++t) {
                int rowCount = rowsPerThread + (t < remainingRows ? 1 : 0);
                if (rowCount == 0) break; // Skip empty threads if height is less than numThreads

                int endRow = startRow + rowCount;
                threads.emplace_back([this, startRow, endRow, xDelta, yDelta, lonExtent, latExtent, &sphereModel]() {
                    double curLat = m_southLatBound + (startRow * latExtent / static_cast<double>(m_destHeight));
                    for (int y = startRow; y < endRow; y++) {
                        float* pDest = m_pDestNoiseMap->GetSlabPtr(0, y);
                        double curLon = m_westLonBound;
                        for (int x = 0; x < m_destWidth; x++) {
                            float curValue = static_cast<float>(sphereModel.GetValue(curLat, curLon));
                            *pDest++ = curValue;
                            curLon += xDelta;
                        }
                        curLat += yDelta;
                        if (m_pCallback) {
                            m_pCallback(y);
                        }
                    }
                    });
                startRow = endRow;
            }

            // Wait for all threads to complete
            for (auto& thread : threads) {
                thread.join();
            }
        }

        void NoiseMapBuilderSphere::SetBounds(double southLatBound, double northLatBound,
            double westLonBound, double eastLonBound) {
            if (southLatBound >= northLatBound || westLonBound >= eastLonBound) {
                throw noise::ExceptionInvalidParam();
            }

            m_southLatBound = southLatBound;
            m_northLatBound = northLatBound;
            m_westLonBound = westLonBound;
            m_eastLonBound = eastLonBound;
        }

        //////////////////////////////////////////////////////////////////////////////
        // RendererImage class

        RendererImage::RendererImage() noexcept :
            m_isLightEnabled(false),
            m_isWrapEnabled(false),
            m_lightAzimuth(45.0),
            m_lightBrightness(1.0),
            m_lightColor(255, 255, 255, 255),
            m_lightContrast(1.0),
            m_lightElev(45.0),
            m_lightIntensity(1.0),
            m_pBackgroundImage(nullptr),
            m_pDestImage(nullptr),
            m_pSourceNoiseMap(nullptr),
            m_recalcLightValues(true) {
            BuildGrayscaleGradient();
        }

        void RendererImage::AddGradientPoint(double gradientPos,
            const Color& gradientColor) {
            m_gradient.AddGradientPoint(gradientPos, gradientColor);
        }

        void RendererImage::BuildGrayscaleGradient() noexcept {
            ClearGradient();
            m_gradient.AddGradientPoint(-1.0, Color(0, 0, 0, 255));
            m_gradient.AddGradientPoint(1.0, Color(255, 255, 255, 255));
        }

        void RendererImage::BuildTerrainGradient() noexcept {
            ClearGradient();
            m_gradient.AddGradientPoint(-1.00, Color(0, 0, 128, 255));
            m_gradient.AddGradientPoint(-0.20, Color(32, 64, 128, 255));
            m_gradient.AddGradientPoint(-0.04, Color(64, 96, 192, 255));
            m_gradient.AddGradientPoint(-0.02, Color(192, 192, 128, 255));
            m_gradient.AddGradientPoint(0.00, Color(0, 192, 0, 255));
            m_gradient.AddGradientPoint(0.25, Color(192, 192, 0, 255));
            m_gradient.AddGradientPoint(0.50, Color(160, 96, 64, 255));
            m_gradient.AddGradientPoint(0.75, Color(128, 255, 255, 255));
            m_gradient.AddGradientPoint(1.00, Color(255, 255, 255, 255));
        }

        Color RendererImage::CalcDestColor(const Color& sourceColor,
            const Color& backgroundColor, double lightValue) const noexcept {
            double sourceRed = static_cast<double>(sourceColor.red) / 255.0;
            double sourceGreen = static_cast<double>(sourceColor.green) / 255.0;
            double sourceBlue = static_cast<double>(sourceColor.blue) / 255.0;
            double sourceAlpha = static_cast<double>(sourceColor.alpha) / 255.0;
            double backgroundRed = static_cast<double>(backgroundColor.red) / 255.0;
            double backgroundGreen = static_cast<double>(backgroundColor.green) / 255.0;
            double backgroundBlue = static_cast<double>(backgroundColor.blue) / 255.0;

            // First, blend the source color to the background color using the alpha
            // of the source color.
            double red = LinearInterp(backgroundRed, sourceRed, sourceAlpha);
            double green = LinearInterp(backgroundGreen, sourceGreen, sourceAlpha);
            double blue = LinearInterp(backgroundBlue, sourceBlue, sourceAlpha);

            if (m_isLightEnabled) {
                // Now calculate the light color.
                double lightRed = lightValue * static_cast<double>(m_lightColor.red) / 255.0;
                double lightGreen = lightValue * static_cast<double>(m_lightColor.green) / 255.0;
                double lightBlue = lightValue * static_cast<double>(m_lightColor.blue) / 255.0;

                // Apply the light color to the new color.
                red *= lightRed;
                green *= lightGreen;
                blue *= lightBlue;
            }

            // Clamp the color channels to the (0..1) range.
            red = std::clamp(red, 0.0, 1.0);
            green = std::clamp(green, 0.0, 1.0);
            blue = std::clamp(blue, 0.0, 1.0);

            // Rescale the color channels to the uint8 (0..255) range and return
            // the new color.
            Color newColor(
                static_cast<uint8>(static_cast<uint>(red * 255.0) & 0xff),
                static_cast<uint8>(static_cast<uint>(green * 255.0) & 0xff),
                static_cast<uint8>(static_cast<uint>(blue * 255.0) & 0xff),
                GetMax(sourceColor.alpha, backgroundColor.alpha));
            return newColor;
        }

        double RendererImage::CalcLightIntensity(double center, double left,
            double right, double down, double up) const noexcept {
            // Recalculate the sine and cosine of the various light values if
            // necessary so it does not have to be calculated each time this method is
            // called.
            if (m_recalcLightValues) {
                m_cosAzimuth = std::cos(m_lightAzimuth * DEG_TO_RAD);
                m_sinAzimuth = std::sin(m_lightAzimuth * DEG_TO_RAD);
                m_cosElev = std::cos(m_lightElev * DEG_TO_RAD);
                m_sinElev = std::sin(m_lightElev * DEG_TO_RAD);
                m_recalcLightValues = false;
            }

            // Now do the lighting calculations.
            constexpr double I_MAX = 1.0;
            double io = I_MAX * SQRT_2 * m_sinElev / 2.0;
            double ix = (I_MAX - io) * m_lightContrast * SQRT_2 * m_cosElev * m_cosAzimuth;
            double iy = (I_MAX - io) * m_lightContrast * SQRT_2 * m_cosElev * m_sinAzimuth;
            double intensity = (ix * (left - right) + iy * (down - up) + io);
            return std::max(intensity, 0.0);
        }

        void RendererImage::ClearGradient() noexcept {
            m_gradient.Clear();
        }

        void RendererImage::Render() {
            if (!m_pSourceNoiseMap
                || !m_pDestImage
                || m_pSourceNoiseMap->GetWidth() <= 0
                || m_pSourceNoiseMap->GetHeight() <= 0
                || m_gradient.GetGradientPointCount() < 2) {
                throw noise::ExceptionInvalidParam();
            }

            int width = m_pSourceNoiseMap->GetWidth();
            int height = m_pSourceNoiseMap->GetHeight();

            // If a background image was provided, make sure it is the same size as the
            // source noise map.
            if (m_pBackgroundImage) {
                if (m_pBackgroundImage->GetWidth() != width
                    || m_pBackgroundImage->GetHeight() != height) {
                    throw noise::ExceptionInvalidParam();
                }
            }

            // Create the destination image.  It is safe to reuse it if this is also the
            // background image.
            if (m_pDestImage != m_pBackgroundImage) {
                m_pDestImage->SetSize(width, height);
            }

            for (int y = 0; y < height; y++) {
                const Color* pBackground = m_pBackgroundImage ? m_pBackgroundImage->GetConstSlabPtr(0, y) : nullptr;
                const float* pSource = m_pSourceNoiseMap->GetConstSlabPtr(0, y);
                Color* pDest = m_pDestImage->GetSlabPtr(0, y);
                for (int x = 0; x < width; x++) {
                    // Get the color based on the value at the current point in the noise
                    // map.
                    Color destColor = m_gradient.GetColor(*pSource);

                    // If lighting is enabled, calculate the light intensity based on the
                    // rate of change at the current point in the noise map.
                    double lightIntensity;
                    if (m_isLightEnabled) {
                        // Calculate the positions of the current point's four-neighbors.
                        int xLeftOffset, xRightOffset;
                        int yUpOffset, yDownOffset;
                        if (m_isWrapEnabled) {
                            xLeftOffset = (x == 0) ? (width - 1) : -1;
                            xRightOffset = (x == width - 1) ? -(width - 1) : 1;
                            yDownOffset = (y == 0) ? (height - 1) * m_pSourceNoiseMap->GetStride() : -m_pSourceNoiseMap->GetStride();
                            yUpOffset = (y == height - 1) ? -(height - 1) * m_pSourceNoiseMap->GetStride() : m_pSourceNoiseMap->GetStride();
                        } else {
                            xLeftOffset = (x == 0) ? 0 : -1;
                            xRightOffset = (x == width - 1) ? 0 : 1;
                            yDownOffset = (y == 0) ? 0 : -m_pSourceNoiseMap->GetStride();
                            yUpOffset = (y == height - 1) ? 0 : m_pSourceNoiseMap->GetStride();
                        }

                        // Get the noise value of the current point in the source noise map
                        // and the noise values of its four-neighbors.
                        double nc = static_cast<double>(*pSource);
                        double nl = static_cast<double>(*(pSource + xLeftOffset));
                        double nr = static_cast<double>(*(pSource + xRightOffset));
                        double nd = static_cast<double>(*(pSource + yDownOffset));
                        double nu = static_cast<double>(*(pSource + yUpOffset));

                        // Now we can calculate the lighting intensity.
                        lightIntensity = CalcLightIntensity(nc, nl, nr, nd, nu);
                        lightIntensity *= m_lightBrightness;
                    } else {
                        // These values will apply no lighting to the destination image.
                        lightIntensity = 1.0;
                    }

                    // Get the current background color from the background image.
                    Color backgroundColor(255, 255, 255, 255);
                    if (m_pBackgroundImage) {
                        backgroundColor = *pBackground;
                    }

                    // Blend the destination color, background color, and the light
                    // intensity together, then update the destination image with that
                    // color.
                    *pDest = CalcDestColor(destColor, backgroundColor, lightIntensity);

                    // Go to the next point.
                    ++pSource;
                    ++pDest;
                    if (m_pBackgroundImage) {
                        ++pBackground;
                    }
                }
            }
        }

        void RendererImage::SetLightContrast(double lightContrast) {
            if (lightContrast <= 0.0) {
                throw noise::ExceptionInvalidParam();
            }
            m_lightContrast = lightContrast;
            m_recalcLightValues = true;
        }

        void RendererImage::SetLightIntensity(double lightIntensity) {
            if (lightIntensity < 0.0) {
                throw noise::ExceptionInvalidParam();
            }
            m_lightIntensity = lightIntensity;
            m_recalcLightValues = true;
        }

        //////////////////////////////////////////////////////////////////////////////
        // RendererNormalMap class

        RendererNormalMap::RendererNormalMap() noexcept :
            m_bumpHeight(1.0),
            m_isWrapEnabled(false),
            m_pDestImage(nullptr),
            m_pSourceNoiseMap(nullptr) {
        }

        Color RendererNormalMap::CalcNormalColor(double nc, double nr, double nu,
            double bumpHeight) const noexcept {
            // Calculate the surface normal.
            nc *= bumpHeight;
            nr *= bumpHeight;
            nu *= bumpHeight;
            double ncr = (nc - nr);
            double ncu = (nc - nu);
            double d = std::sqrt((ncu * ncu) + (ncr * ncr) + 1);
            double vxc = (nc - nr) / d;
            double vyc = (nc - nu) / d;
            double vzc = 1.0 / d;

            // Map the normal range from the (-1.0 .. +1.0) range to the (0 .. 255)
            // range.
            uint8 xc = static_cast<uint8>(static_cast<uint>(std::floor((vxc + 1.0) * 127.5)) & 0xff);
            uint8 yc = static_cast<uint8>(static_cast<uint>(std::floor((vyc + 1.0) * 127.5)) & 0xff);
            uint8 zc = static_cast<uint8>(static_cast<uint>(std::floor((vzc + 1.0) * 127.5)) & 0xff);

            return Color(xc, yc, zc, 0);
        }

        void RendererNormalMap::Render() {
            if (!m_pSourceNoiseMap
                || !m_pDestImage
                || m_pSourceNoiseMap->GetWidth() <= 0
                || m_pSourceNoiseMap->GetHeight() <= 0) {
                throw noise::ExceptionInvalidParam();
            }

            int width = m_pSourceNoiseMap->GetWidth();
            int height = m_pSourceNoiseMap->GetHeight();

            for (int y = 0; y < height; y++) {
                const float* pSource = m_pSourceNoiseMap->GetConstSlabPtr(0, y);
                Color* pDest = m_pDestImage->GetSlabPtr(0, y);
                for (int x = 0; x < width; x++) {
                    // Calculate the positions of the current point's right and up
                    // neighbors.
                    int xRightOffset, yUpOffset;
                    if (m_isWrapEnabled) {
                        xRightOffset = (x == width - 1) ? -(width - 1) : 1;
                        yUpOffset = (y == height - 1) ? -(height - 1) * m_pSourceNoiseMap->GetStride() : m_pSourceNoiseMap->GetStride();
                    } else {
                        xRightOffset = (x == width - 1) ? 0 : 1;
                        yUpOffset = (y == height - 1) ? 0 : m_pSourceNoiseMap->GetStride();
                    }

                    // Get the noise value of the current point in the source noise map
                    // and the noise values of its right and up neighbors.
                    double nc = static_cast<double>(*pSource);
                    double nr = static_cast<double>(*(pSource + xRightOffset));
                    double nu = static_cast<double>(*(pSource + yUpOffset));

                    // Calculate the normal product.
                    *pDest = CalcNormalColor(nc, nr, nu, m_bumpHeight);

                    // Go to the next point.
                    ++pSource;
                    ++pDest;
                }
            }
        }

        void RendererNormalMap::EnableWrap(bool enable) noexcept {
            m_isWrapEnabled = enable;
        }

        void RendererNormalMap::SetBumpHeight(double bumpHeight) noexcept {
            m_bumpHeight = bumpHeight;
        }

        void RendererNormalMap::SetDestImage(Image& destImage) noexcept {
            m_pDestImage = &destImage;
        }

        void RendererNormalMap::SetSourceNoiseMap(const NoiseMap& sourceNoiseMap) noexcept {
            m_pSourceNoiseMap = &sourceNoiseMap;
        }

    } // namespace utils

} // namespace noise