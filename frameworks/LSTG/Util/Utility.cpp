﻿#include "Utility.h"
#include "AppFrame.h"
#include "LogSystem.h"
#include "renderer/backend/Device.h"
#include "renderer/backend/gfx/TextureGFX.h"
#include <locale>
#include <codecvt>
#include <cstring>

using namespace std;
using namespace lstg;
using namespace cocos2d;

string lstg::StringFormat(const char* Format, ...)noexcept
{
	va_list vaptr;
	va_start(vaptr, Format);
	auto ret = StringFormatV(Format, vaptr);
	va_end(vaptr);
	return ret;
}

string lstg::StringFormatV(const char* Format, va_list vaptr)noexcept
{
	string tRet;
	try
	{
		while (*Format != '\0')
		{
			char c = *Format;
			if (c != '%')
				tRet.push_back(c);
			else
			{
				c = *(++Format);

				switch (c)
				{
				case '%':
					tRet.push_back('%');
					break;
				case 'b':
					tRet.append(va_arg(vaptr, bool) ? "true" : "false");
					break;
				case 'd':
					tRet.append(to_string(va_arg(vaptr, int32_t)));
					break;
				case 'f':
					tRet.append(to_string(va_arg(vaptr, double)));
					break;
				case 'l':
					c = *(++Format);
					switch (c)
					{
					case 'f':
						tRet.append(to_string(va_arg(vaptr, double)));
						break;
					case 'd':
						tRet.append(to_string(va_arg(vaptr, int64_t)));
						break;
					case 'u':
						tRet.append(to_string(va_arg(vaptr, uint64_t)));
						break;
					default:
						tRet.append("%l");
						if (c == '\0')
							Format--;
						else
							tRet.push_back(c);
						break;
					}
					break;
				case 'u':
					tRet.append(to_string(va_arg(vaptr, uint32_t)));
					break;
				case 'p':
					tRet.append(to_string(va_arg(vaptr, unsigned int)));
					break;
				case 'c':
					tRet.push_back(va_arg(vaptr, int32_t));
					break;
				case 's':
					{
						const char* p = va_arg(vaptr, char*);
						if (p)
							tRet.append(p);
						else
							tRet.append("<null>");
					}
					break;
				case 'w':
					{
						const wchar_t* p = va_arg(vaptr, wchar_t*);
						if (p)
							tRet.append(move(util::WideCharToMultiByte_UTF8(p)));
						else
							tRet.append("<null>");
					}
					break;
				default:
					tRet.push_back('%');
					if (c == '\0')
						Format--;
					else
						tRet.push_back(c);
					break;
				}
			}
			Format++;
		}
	}
	catch (const bad_alloc&)
	{
	}

	return tRet;
}

std::string lstg::stackDump(lua_State *L)
{
	const auto top = lua_gettop(L);
	string s;
	for (auto i = 1; i <= top; i++) {
		s += "[" + to_string(i) + "] = ";
		const auto t = lua_type(L, i);
		switch (t) {
		case LUA_TSTRING:
			s += "\'";
			s += lua_tostring(L, i);
			s += "\'";
			break;
		case LUA_TBOOLEAN:
			if (lua_toboolean(L, i))
				s += "true";
			else
				s += "false";
			break;
		case LUA_TNUMBER:
			s += to_string(lua_tonumber(L, i));
			break;
		default:
			s += lua_typename(L, t);
			break;
		}
		s += "\n";
	}
	return s;
}

string lstg::ReplacePathSep(string path, const string& ori, const string& dst)
{
	auto ret = path;
	size_t pos;
	while ((pos = ret.find_first_of(ori)) != string::npos)
	{
		ret.replace(pos, 1, dst);
	}
	return ret;
}

Vec2 lstg::AlignmentToAnchorPoint(TextHAlignment ha, TextVAlignment va)
{
	Vec2 ret;
	switch (ha)
	{
	case TextHAlignment::LEFT: ret.x = 0.f; break;
	case TextHAlignment::CENTER: ret.x = 0.5f; break;
	case TextHAlignment::RIGHT: ret.x = 1.f; break;
	}
	switch (va)
	{
	case TextVAlignment::TOP: ret.y = 1.f; break;
	case TextVAlignment::CENTER: ret.y = 0.5f; break;
	case TextVAlignment::BOTTOM: ret.y = 0.f; break;
	}
	return ret;
}

template <typename T>
void _pathUniform(T begin, T end, bool forward_slash, bool lower)
{
	while (begin != end)
	{
		int c = *begin;
		if (c == 0)
			break;
		if (lower)
		{
			if (c >= 'A' && c <= 'Z')
				*begin = c - 'A' + 'a';
		}
		else
		{
			if (c >= 'a' && c <= 'z')
				*begin = c - 'a' + 'A';
		}
		if (forward_slash)
		{
			if (c == '\\')
				*begin = '/';
		}
		else
		{
			if (c == '/')
				*begin = '\\';
		}
		++begin;
	}
}

void lstg::pathUniform(string& path, bool forward_slash, bool lower, bool utf8_convert)
{
	if (path[0] == '.' && (path[1] == '/' || path[1] == '\\'))path = path.substr(2);
	if (utf8_convert)
	{
		auto s = util::MultiByteToWideChar_UTF8(path);
		_pathUniform(s.begin(), s.end(), forward_slash, lower);
		path = util::WideCharToMultiByte_UTF8(s);
	}
	else
		_pathUniform(path.begin(), path.end(), forward_slash, lower);
}

void lstg::pathUniform(wstring& path, bool forward_slash, bool lower, bool utf8_convert)
{
	if (path[0] == L'.' && (path[1] == L'/' || path[1] == L'\\'))path = path.substr(2);
	if (utf8_convert)
	{
		auto s = util::WideCharToMultiByte_UTF8(path);
		_pathUniform(s.begin(), s.end(), forward_slash, lower);
		path = util::MultiByteToWideChar_UTF8(s);
	}
	else
		_pathUniform(path.begin(), path.end(), forward_slash, lower);
}

string lstg::BlendFactor_tostring(backend::BlendFactor factor)
{
	switch (factor)
	{
	case backend::BlendFactor::ZERO:return "ZERO";
	case backend::BlendFactor::ONE:return "ONE";
	case backend::BlendFactor::SRC_COLOR:return "SRC_COLOR";
	case backend::BlendFactor::ONE_MINUS_SRC_COLOR:return "ONE_MINUS_SRC_COLOR";
	case backend::BlendFactor::SRC_ALPHA:return "SRC_ALPHA";
	case backend::BlendFactor::ONE_MINUS_SRC_ALPHA:return "ONE_MINUS_SRC_ALPHA";
	case backend::BlendFactor::DST_COLOR:return "DST_COLOR";
	case backend::BlendFactor::ONE_MINUS_DST_COLOR:return "ONE_MINUS_DST_COLOR";
	case backend::BlendFactor::DST_ALPHA:return "DST_ALPHA";
	case backend::BlendFactor::ONE_MINUS_DST_ALPHA:return "ONE_MINUS_DST_ALPHA";
	case backend::BlendFactor::CONSTANT_ALPHA:return "CONSTANT_ALPHA";
	case backend::BlendFactor::SRC_ALPHA_SATURATE:return "SRC_ALPHA_SATURATE";
	case backend::BlendFactor::ONE_MINUS_CONSTANT_ALPHA:return "ONE_MINUS_CONSTANT_ALPHA";
	case backend::BlendFactor::BLEND_CLOLOR:return "BLEND_CLOLOR";
	default: ;
	}
	return "UNKNOWN";
}

string lstg::tostring(const Vec2& v)
{
	return StringUtils::format("(%.3f, %.3f)", v.x, v.y);
}

string lstg::tostring(const Vec3& v)
{
	return StringUtils::format("(%.3f, %.3f, %.3f)", v.x, v.y, v.z);
}

string lstg::tostring(const Vec4& v)
{
	return StringUtils::format("(%.3f, %.3f, %.3f, %.3f)", v.x, v.y, v.z, v.w);
}

string lstg::tostring(const Rect& r)
{
	return "(o = " + tostring(r.origin) + ", s = " + tostring(r.size) + ")";
}

string lstg::tostring(const Color3B& c)
{
	return "(" +
		to_string(c.r) + ", " +
		to_string(c.g) + ", " +
		to_string(c.b) + ")";
}

string lstg::tostring(const Color4B& c)
{
	return "(" +
		to_string(c.a) + ", " +
		to_string(c.r) + ", " +
		to_string(c.g) + ", " +
		to_string(c.b) + ")";
}

string lstg::tostring(const Color4F& c)
{
	return "(" +
		to_string(c.a) + ", " +
		to_string(c.r) + ", " +
		to_string(c.g) + ", " +
		to_string(c.b) + ")";
}

string lstg::tostring(const BlendFunc& b)
{
	return "(src = " + BlendFactor_tostring(b.src) + ", dst = " + BlendFactor_tostring(b.dst) + ")";
}

string lstg::tostring(const V3F_C4B_T2F& v)
{
	string ret = StringUtils::format("uv: (%.3f, %.3f)", v.texCoords.u, v.texCoords.v);
	ret += ", vert: " + tostring(v.vertices);
	ret += ", c: " + tostring(v.colors);
	return ret;
}

string lstg::tostring(const V3F_C4B_T2F_Quad& v)
{
	string ret = "tl: " + tostring(v.tl);
	ret += "\ntr: " + tostring(v.tr);
	ret += "\nbl: " + tostring(v.bl);
	ret += "\nbr: " + tostring(v.br);
	return ret;
}

string lstg::tostring(const Mat4& m)
{
	return StringFormat("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f",
		m.m[0], m.m[4], m.m[8], m.m[12],
		m.m[1], m.m[5], m.m[9], m.m[13],
		m.m[2], m.m[6], m.m[10], m.m[14],
		m.m[3], m.m[7], m.m[11], m.m[15]);
}

string lstg::tostring(TextHAlignment ha)
{
	switch (ha)
	{
	case TextHAlignment::LEFT: return "left";
	case TextHAlignment::CENTER: return "center";
	case TextHAlignment::RIGHT: return "right";
	default: ;
	}
	return "";
}

string lstg::tostring(TextVAlignment va)
{
	switch (va)
	{
	case TextVAlignment::TOP: return "top";
	case TextVAlignment::CENTER: return "center";
	case TextVAlignment::BOTTOM: return "bottom";
	default: ;
	}
	return "";
}

void lstg::transformPoint(Vec3& point, const Vec2& anchorPIP,
	float x, float y, float rot, float hscale, float vscale, float z)
{
	float zz2_;
	float wz2;
	if (rot == 0.f)
	{
		zz2_ = 1.0f;
		wz2 = 0;
	}
	else
	{
		const float halfRadz = -(.5f * 0.01745329252f * rot);
		const float qz = std::sin(halfRadz);
		const float qw = std::cos(halfRadz);
		const float z2 = qz + qz;
		zz2_ = 1.0f - qz * z2;
		wz2 = qw * z2;
	}

	const float dx = (point.x - anchorPIP.x) * hscale;
	const float dy = (point.y - anchorPIP.y) * vscale;

	point.x = dx * zz2_ + dy * (-wz2) + x;
	point.y = dx * wz2 + dy * zz2_ + y;
	point.z += z;
}

void lstg::transformQuad(V3F_C4B_T2F_Quad& quad, const cocos2d::Vec2& anchorPIP, float x, float y, float rot,
	float hscale, float vscale, float z)
{
	float zz2_;
	float wz2;
	if (rot == 0.f)
	{
		zz2_ = 1.0f;
		wz2 = 0;
	}
	else
	{
		const float halfRadz = -(.5f * 0.01745329252f * rot);
		const float qz = std::sin(halfRadz);
		const float qw = std::cos(halfRadz);
		const float z2 = qz + qz;
		zz2_ = 1.0f - qz * z2;
		wz2 = qw * z2;
	}
	for (int i = 0; i < 4; ++i)
	{
		auto& point = ((V3F_C4B_T2F*)&quad)[i].vertices;
		const float dx = (point.x - anchorPIP.x) * hscale;
		const float dy = (point.y - anchorPIP.y) * vscale;
		point.x = dx * zz2_ + dy * (-wz2) + x;
		point.y = dx * wz2 + dy * zz2_ + y;
		point.z += z;
	}
}

void lstg::getNodeTransform(const Vec2& anchorPointInPoints,
                            float x, float y, float rot, float hscale, float vscale, float z, Mat4* _transform)
{
	float zz2_;
	float wz2;
	// assert that most rot are 0
	if (rot == 0.f)
	{
		zz2_ = 1.0f;
		wz2 = 0;
	}
	else
	{
		const float halfRadz = -(.5f * 0.01745329252f * rot);
		const float qz = std::sin(halfRadz);
		const float qw = std::cos(halfRadz);
		const float z2 = qz + qz;
		zz2_ = 1.0f - qz * z2;
		wz2 = qw * z2;
	}

	_transform->m[0] = zz2_ * hscale;
	_transform->m[1] = wz2 * hscale;
	_transform->m[2] = 0.0f;
	_transform->m[3] = 0.0f;

	_transform->m[4] = -wz2 * vscale;
	_transform->m[5] = zz2_ * vscale;
	_transform->m[6] = 0.0f;
	_transform->m[7] = 0.0f;

	_transform->m[8] = 0.0f;
	_transform->m[9] = 0.0f;
	_transform->m[10] = 1.0f;
	_transform->m[11] = 0.0f;

	// assert that most anchor points are not 0 (usually center)
	_transform->m[12] = x - _transform->m[0] * anchorPointInPoints.x - _transform->m[4] * anchorPointInPoints.y;
	_transform->m[13] = y - _transform->m[1] * anchorPointInPoints.x - _transform->m[5] * anchorPointInPoints.y;
	_transform->m[14] = z;
	_transform->m[15] = 1.0f;
}

Mat4 lstg::getNodeTransform(const Vec2& anchorPointInPoints,
	float x, float y, float rot, float hscale, float vscale, float z)
{
	Mat4 ret;
	getNodeTransform(anchorPointInPoints, x, y, rot, hscale, vscale, z, &ret);
	return ret;
}

void lstg::getQuaternion(const Vec3& axis, float angle, Quaternion* out)
{
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;
	float n = x * x + y * y + z * z;
	if (n != 1.f)
	{
		n = std::sqrt(n);
		if (n > MATH_TOLERANCE)
		{
			x /= n;
			y /= n;
			z /= n;
		}
	}
	const float halfAngle = angle * 0.5f;
	const float s = std::sin(halfAngle);
	const float c = std::cos(halfAngle);
	out->x = x * s;
	out->y = y * s;
	out->z = z * s;
	out->w = c;
}

void lstg::getRotationTransform(const Quaternion& q, Mat4* out)
{
	const float x2 = q.x + q.x;
	const float y2 = q.y + q.y;
	const float z2 = q.z + q.z;

	const float xx2 = q.x * x2;
	const float yy2 = q.y * y2;
	const float zz2 = q.z * z2;
	const float xy2 = q.x * y2;
	const float xz2 = q.x * z2;
	const float yz2 = q.y * z2;
	const float wx2 = q.w * x2;
	const float wy2 = q.w * y2;
	const float wz2 = q.w * z2;

	out->m[0] = 1.0f - yy2 - zz2;
	out->m[1] = xy2 + wz2;
	out->m[2] = xz2 - wy2;
	out->m[3] = 0.0f;

	out->m[4] = xy2 - wz2;
	out->m[5] = 1.0f - xx2 - zz2;
	out->m[6] = yz2 + wx2;
	out->m[7] = 0.0f;

	out->m[8] = xz2 + wy2;
	out->m[9] = yz2 - wx2;
	out->m[10] = 1.0f - xx2 - yy2;
	out->m[11] = 0.0f;

	out->m[12] = 0.0f;
	out->m[13] = 0.0f;
	out->m[14] = 0.0f;
	out->m[15] = 1.0f;
}

void lstg::getRotationTransform(const Vec3& axis, float angle, Mat4* out)
{
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;
	// Make sure the input axis is normalized.
	float n = x * x + y * y + z * z;
	if (n != 1.0f)
	{
		n = std::sqrt(n);
		if (n > MATH_TOLERANCE)
		{
			n = 1.0f / n;
			x *= n;
			y *= n;
			z *= n;
		}
	}

	const float c = std::cos(angle);
	const float s = std::sin(angle);

	const float t = 1.0f - c;
	const float tx = t * x;
	const float ty = t * y;
	const float tz = t * z;
	const float txy = tx * y;
	const float txz = tx * z;
	const float tyz = ty * z;
	const float sx = s * x;
	const float sy = s * y;
	const float sz = s * z;

	out->m[0] = c + tx * x;
	out->m[1] = txy + sz;
	out->m[2] = txz - sy;
	out->m[3] = 0.0f;

	out->m[4] = txy - sz;
	out->m[5] = c + ty * y;
	out->m[6] = tyz + sx;
	out->m[7] = 0.0f;

	out->m[8] = txz + sy;
	out->m[9] = tyz - sx;
	out->m[10] = c + tz * z;
	out->m[11] = 0.0f;

	out->m[12] = 0.0f;
	out->m[13] = 0.0f;
	out->m[14] = 0.0f;
	out->m[15] = 1.0f;
}

void lstg::getNodeTransform3D(const Vec2& anchorPointInPoints, float x, float y, float z,
	const Quaternion& q, float hscale, float vscale, float zscale, Mat4* _transform)
{
	getRotationTransform(q, _transform);
	_transform->m[12] = x;
	_transform->m[13] = y;
	_transform->m[14] = z;
	if (hscale != 1.f)
	{
		_transform->m[0] *= hscale;
		_transform->m[1] *= hscale;
		_transform->m[2] *= hscale;
	}
	if (vscale != 1.f)
	{
		_transform->m[4] *= vscale;
		_transform->m[5] *= vscale;
		_transform->m[6] *= vscale;
	}
	if (zscale != 1.f)
	{
		_transform->m[8] *= zscale;
		_transform->m[9] *= zscale;
		_transform->m[10] *= zscale;
	}
	if (!anchorPointInPoints.isZero())
	{
		_transform->m[12] -= _transform->m[0] * anchorPointInPoints.x + _transform->m[4] * anchorPointInPoints.y;
		_transform->m[13] -= _transform->m[1] * anchorPointInPoints.x + _transform->m[5] * anchorPointInPoints.y;
		_transform->m[14] -= _transform->m[2] * anchorPointInPoints.x + _transform->m[6] * anchorPointInPoints.y;
	}
}

void lstg::getNodeTransform3D(const Vec2& anchorPointInPoints, float x, float y, float z,
	const Vec3& axis, float angle, float hscale, float vscale, float zscale, Mat4* _transform)
{
	getRotationTransform(axis, angle, _transform);
	_transform->m[12] = x;
	_transform->m[13] = y;
	_transform->m[14] = z;
	if (hscale != 1.f)
	{
		_transform->m[0] *= hscale;
		_transform->m[1] *= hscale;
		_transform->m[2] *= hscale;
	}
	if (vscale != 1.f)
	{
		_transform->m[4] *= vscale;
		_transform->m[5] *= vscale;
		_transform->m[6] *= vscale;
	}
	if (zscale != 1.f)
	{
		_transform->m[8] *= zscale;
		_transform->m[9] *= zscale;
		_transform->m[10] *= zscale;
	}
	if (!anchorPointInPoints.isZero())
	{
		_transform->m[12] -= _transform->m[0] * anchorPointInPoints.x + _transform->m[4] * anchorPointInPoints.y;
		_transform->m[13] -= _transform->m[1] * anchorPointInPoints.x + _transform->m[5] * anchorPointInPoints.y;
		_transform->m[14] -= _transform->m[2] * anchorPointInPoints.x + _transform->m[6] * anchorPointInPoints.y;
	}
}

Image* lstg::getTextureImage(Texture2D* texture, bool flipImage)
{
	if (!texture)
		return nullptr;
	const auto s = texture->getContentSizeInPixels();
	return getTextureImage(texture, 0, 0, s.width, s.height, flipImage);
}

Image* lstg::getTextureImage(Texture2D* texture,
	size_t x, size_t y, size_t width, size_t height, bool flipImage)
{
	if (!texture || width * height == 0)
		return nullptr;
	auto format = texture->getPixelFormat();
	if (format == backend::PixelFormat::NONE)
		format = texture->getBackendTexture()->getTextureFormat();
	if(format != backend::PixelFormat::RGBA8888)
		return nullptr;
	const auto image = new Image();
	const auto bytePerPixel = texture->getBitsPerPixelForFormat() / 8;
	auto callback = [=](const unsigned char* data, std::size_t w, std::size_t h) {
		image->initWithRawData(data, w * h * bytePerPixel,
			w, h, bytePerPixel * 8, texture->hasPremultipliedAlpha());
	};
	//texture->getBackendTexture()->getBytes(x, y, width, height, flipImage, callback);
	((cocos2d::backend::Texture2DGFX*)texture->getBackendTexture())->getBytes(
		x, y, width, height, flipImage, callback);
	return image;
}

Image* lstg::getSpriteImage(Sprite* sprite, bool flipImage)
{
	if (!sprite || !sprite->getTexture())
		return nullptr;
	const auto rect = sprite->getTextureRect();
	return getTextureImage(sprite->getTexture(),
		rect.origin.x, rect.origin.y, rect.size.width, rect.size.height, flipImage);
}

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "../../external/nanosvg/src/nanosvg.h"
#include "../../external/nanosvg/src/nanosvgrast.h"
bool lstg::initImageWithSVG(Image* image, const std::string& path,
	float scale, const std::string& units, float dpi)
{
	if (!image)
		return false;
	bool ok = false;
	NSVGimage* img = nullptr;
	NSVGrasterizer* rast = nullptr;
	do
	{
		const auto str = FileUtils::getInstance()->getStringFromFile(path);
		if (str.empty()) break;
		img = nsvgParse((char*)str.c_str(), units.c_str(), dpi);
		if (!img) break;
		rast = nsvgCreateRasterizer();
		if (!rast) break;
		const auto w = (int)(img->width * scale);
		const auto h = (int)(img->height * scale);
		const auto byteSize = w * h * 4;
		auto buffer = new (std::nothrow) uint8_t[byteSize];
		if (!buffer) break;
		nsvgRasterize(rast, img, 0, 0, scale, buffer, w, h, w * 4);
		ok = image->initWithRawData(buffer, byteSize, w, h, 32, false);
		delete[] buffer;
	} while (false);
	nsvgDeleteRasterizer(rast);
	nsvgDelete(img);
	return ok;
}

bool lstg::initImageWithSVG(Image* image, const std::string& path, const Size& size)
{
	const auto width = (int)size.width;
	const auto height = (int)size.height;
	if (width <= 0 && height <= 0)
		return initImageWithSVG(image, path, 1, "px", 96);
	if (!image)
		return false;
	bool ok = false;
	NSVGimage* img = nullptr;
	NSVGrasterizer* rast = nullptr;
	do
	{
		const auto str = FileUtils::getInstance()->getStringFromFile(path);
		if (str.empty()) break;
		img = nsvgParse((char*)str.c_str(), "px", 96);
		if (!img) break;
		rast = nsvgCreateRasterizer();
		if (!rast) break;
		int w = 0, h = 0, dx = 0, dy = 0;
		float scale = 1;
		if (width <= 0)
		{
			scale = (float)height / img->height;
			h = height;
			w = int(scale * img->width);
		}
		else if (height <= 0)
		{
			scale = (float)width / img->width;
			w = width;
			h = int(scale * img->height);
		}
		else
		{
			scale = std::min((float)height / img->height, (float)width / img->width);
			w = width;
			h = height;
			dx = (w - img->width * scale) / 2;
			dy = (h - img->height * scale) / 2;
		}
		const auto byteSize = w * h * 4;
		auto buffer = new (std::nothrow) uint8_t[byteSize];
		if (!buffer) break;
		nsvgRasterize(rast, img, dx, dy, scale, buffer, w, h, w * 4);
		ok = image->initWithRawData(buffer, byteSize, w, h, 32, false);
		delete[] buffer;
	} while (false);
	nsvgDeleteRasterizer(rast);
	nsvgDelete(img);
	return ok;
}

void lstg::deployThreadTask(size_t taskSize, size_t nSlice, const std::function<void(int, int, int)>& task)
{
	const auto nThr = nSlice;
	const int num = taskSize / nThr;
	for (auto i = 0u; i < nThr; ++i)
	{
		const auto start = i * num;
		auto end = (i + 1)*num;
		if (i == nThr - 1)
			end = taskSize;
		auto task_ = [=]()
		{
			task(start, end, i);
		};
		if (i == nThr - 1) // reserve one job for this thread
			task_();
		else
			LTHP.add_task(task_);
	}
}

void lstg::deployThreadTaskAndWait(size_t taskSize, size_t nSlice, const std::function<void(int, int, int)>& task)
{
	const auto nThr = nSlice;
	const int num = taskSize / nThr;
	vector<future<void>> futures;
	for (auto i = 0u; i < nThr; ++i)
	{
		const auto start = i * num;
		auto end = (i + 1)*num;
		if (i == nThr - 1)
			end = taskSize;
		auto task_ = [=]()
		{
			task(start, end, i);
		};
		if (i == nThr - 1) // reserve one job for this thread
			task_();
		else
			futures.emplace_back(LTHP.add_task_future(task_));
	}
	for (auto& fu : futures)
		fu.get();
}

std::vector<std::future<std::shared_ptr<void>>> lstg::deployThreadTaskFuture(size_t taskSize, size_t nSlice,
	const std::function<std::shared_ptr<void>(int, int, int)>& task)
{
	const auto nThr = nSlice;
	const int num = taskSize / nThr;
	vector<future<shared_ptr<void>>> futures;
	for (auto i = 0u; i < nThr; ++i)
	{
		const auto start = i * num;
		auto end = (i + 1)*num;
		if (i == nThr - 1)
			end = taskSize;
		std::function<shared_ptr<void>()> task_ = [=]()
		{
			return task(start, end, i);
		};
		if (i == nThr - 1) // reserve one job for this thread
		{
			packaged_task<shared_ptr<void>()> _task(task_);
			futures.emplace_back(_task.get_future());
			_task();
		}
		else
			futures.emplace_back(LTHP.add_task_future(task_));
	}
	return futures;
}

#include "xxhash.h"
uint32_t lstg::XXHash32(const void* data, size_t size, uint32_t seed)
{
	return XXH32(data, size, seed);
}

uint64_t lstg::XXHash64(const void* data, size_t size, uint64_t seed)
{
	return XXH64(data, size, seed);
}

void lstg::RC4XOR(const std::string& key, const void* data, size_t size, void* out)
{
	if (!data || !out)
		return;
	RC4 rc4(key.c_str(), key.size());
	rc4((const uint8_t*)data, size, (uint8_t*)out);
}

#include "md5/md5.h"
std::string lstg::MD5Hash(const void* data, size_t size)
{
	if (!data || size == 0)
		return "";
	constexpr unsigned int MD5_DIGEST_LENGTH = 16;
	md5_state_t state;
	md5_byte_t digest[MD5_DIGEST_LENGTH];
	char hexOutput[(MD5_DIGEST_LENGTH << 1) + 1] = { 0 };
	md5_init(&state);
	size_t remain = size;
	auto current = (const md5_byte_t*)data;
	while (remain > std::numeric_limits<int>::max())
	{
		md5_append(&state, (const md5_byte_t*)current, std::numeric_limits<int>::max());
		current += std::numeric_limits<int>::max();
		remain -= std::numeric_limits<int>::max();
	}
	md5_append(&state, (const md5_byte_t*)current, remain);
	md5_finish(&state, digest);
	for (int di = 0; di < 16; ++di)
		sprintf(hexOutput + di * 2, "%02x", digest[di]);
	return hexOutput;
}

void RC4::operator()(const uint8_t* input, size_t inputlen, uint8_t* output)
{
	uint8_t Scpy[256];
	std::memcpy(Scpy, S, sizeof(S));
	for (size_t i = 0, j = 0; i < inputlen; i++)
	{
		const size_t i2 = (i + 1) % 256;
		j = (j + Scpy[i2]) % 256;
		swap(Scpy[i2], Scpy[j]);
		const uint8_t n = Scpy[(Scpy[i2] + Scpy[j]) % 256];
		*(output + i) = *(input + i) ^ n;
	}
}

RC4::RC4(const char* password, size_t len)
	: S()
{
	len = min(len, size_t(256));
	for (int i = 0; i < 256; ++i)
		S[i] = i;
	for (size_t i = 0, j = 0; i < 256; i++)
	{
		j = (j + S[i] + password[i % len]) % 256;
		swap(S[i], S[j]);
	}
}

Viewport util::Viewport(float left, float right, float bottom, float top)
{
	::Viewport ret;
	ret.x = std::min(left, right);
	ret.y = std::min(bottom, top);
	ret.w = std::max(left, right) - ret.x;
	ret.h = std::max(bottom, top) - ret.y;
	return ret;
}

#define MAP(K) { #K, backend::BlendOperation::K }
static std::unordered_map<std::string, backend::BlendOperation> BlendOperationMap = {
	MAP(ADD),
	MAP(SUBTRACT),
	MAP(RESERVE_SUBTRACT)
};
#undef MAP

vector<string> util::stringSplit(const string& str, const string& delimiter, bool trim)
{
	vector<string> ret;
	ret.clear();

	uint32_t last = 0;
	auto pos = str.find(delimiter);
	while (pos != string::npos)
	{
		auto tstr = str.substr(last, pos - last);
		if (!(trim && tstr.empty()))
			ret.push_back(tstr);

		last = pos + (int32_t)delimiter.length();
		pos = (int32_t)str.find(delimiter, last);
	}
	const auto tstr = str.substr(last, pos - last);
	if (!(trim && tstr.empty()))
		ret.push_back(tstr);

	return ret;
}

string util::stringTrimLeft(const string &str)
{
	auto ret = str;
	const auto p = find_if_not(ret.begin(), ret.end(), ::isspace);
	ret.erase(ret.begin(), p);
	return ret;
}

string util::stringTrimRight(const string &str)
{
	auto ret = str;
	const auto p = find_if_not(ret.rbegin(), ret.rend(), ::isspace);
	ret.erase(p.base(), ret.end());
	return ret;
}

string util::stringTrim(const string &str)
{
	return stringTrimLeft(stringTrimRight(str));
}

wstring_convert<codecvt_utf8<wchar_t>> cv;

wstring util::MultiByteToWideChar_UTF8(const string& str)
{
	wstring ret;
	try
	{
		ret = cv.from_bytes(str);
	}
	catch (const range_error&)
	{
		ret = L"";
	}
	return ret;
}

string util::WideCharToMultiByte_UTF8(const wstring& str)
{
	string ret;
	try
	{
		ret = cv.to_bytes(str);
	}
	catch (const range_error&)
	{
		ret = "";
	}
	return ret;
}

bool util::BlendOperationFromString(const std::string& str, backend::BlendOperation& val)
{
	const auto it = BlendOperationMap.find(str);
	if (it == BlendOperationMap.end())
		return false;
	val = it->second;
	return true;
}

#define MAP(K) { #K, backend::BlendFactor::K }
static std::unordered_map<std::string, backend::BlendFactor> BlendFactorMap = {
	MAP(ZERO),
	MAP(ONE),
	MAP(SRC_COLOR),
	MAP(ONE_MINUS_SRC_COLOR),
	MAP(SRC_ALPHA),
	MAP(ONE_MINUS_SRC_ALPHA),
	MAP(DST_COLOR),
	MAP(ONE_MINUS_DST_COLOR),
	MAP(DST_ALPHA),
	MAP(ONE_MINUS_DST_ALPHA),
	MAP(CONSTANT_ALPHA),
	MAP(SRC_ALPHA_SATURATE),
	MAP(ONE_MINUS_CONSTANT_ALPHA),
	MAP(BLEND_CLOLOR)
};
#undef MAP

bool util::BlendFactorFromString(const std::string& str, backend::BlendFactor& val)
{
	const auto it = BlendFactorMap.find(str);
	if (it == BlendFactorMap.end())
		return false;
	val = it->second;
	return true;
}

backend::Program* util::CreateProgramFromPath(const std::string& v, const std::string& f)
{	
	auto fu = FileUtils::getInstance();
	const auto v_content = fu->getStringFromFile(fu->fullPathForFilename(v));
	const auto f_content = fu->getStringFromFile(fu->fullPathForFilename(f));
	const auto ret = backend::Device::getInstance()->newProgram(v_content, f_content);
	if (ret)
	{
		ret->autorelease();
		return ret;
	}
	CheckVertexShader(v_content);
	CheckFragmentShader(f_content);
	return nullptr;
}

backend::Program* util::CreateProgramFromString(const std::string& v, const std::string& f)
{
	const auto ret = backend::Device::getInstance()->newProgram(v, f);
	if (ret)
	{
		ret->autorelease();
		return ret;
	}
	CheckVertexShader(v);
	CheckFragmentShader(f);
	return nullptr;
}

backend::Program* util::CreateProgramFromData(Data* v, Data* f)
{
	if (!v || !f)
	{
		XERROR("invalid param");
		return nullptr;
	}
	auto vdata = (char*)v->getBytes();
	auto fdata = (char*)f->getBytes();
	if (vdata[v->getSize() - 1] != '\0' || fdata[f->getSize() - 1] != '\0')
	{
		XERROR("invalid data");
		return nullptr;
	}
	const auto ret = backend::Device::getInstance()->newProgram(vdata, fdata);
	if (ret)
	{
		ret->autorelease();
		return ret;
	}
	XERROR("can't load shader from given bytes");
	CheckVertexShader(vdata);
	CheckFragmentShader(fdata);
	return nullptr;
}
