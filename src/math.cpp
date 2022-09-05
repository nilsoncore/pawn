#include "math.h"

Vec2f new_vec2f(float x, float y) {
    Vec2f result = { x, y };
    return result;
}

Vec2f new_vec2f(float xy) {
    Vec2f result = { xy, xy };
    return result;
}

Vec3f new_vec3f(float x, float y, float z) {
    Vec3f result = { x, y, z };
    return result;
}

Vec3f new_vec3f(float xyz) {
    Vec3f result = { xyz, xyz, xyz };
    return result;
}

Vec4f new_vec4f(float x, float y, float z, float w) {
    Vec4f result = { x, y, z, w };
    return result;
}

Vec4f new_vec4f(float xyzw) {
    Vec4f result = { xyzw, xyzw, xyzw, xyzw };
    return result;
}

Vec2i new_vec2i(int x, int y) {
    Vec2i result = { x, y };
    return result;
}

Vec2i new_vec2i(int xy) {
    Vec2i result = { xy, xy };
    return result;
}

Vec3i new_vec3i(int x, int y, int z) {
    Vec3i result = { x, y, z };
    return result;
}

Vec3i new_vec3i(int xyz) {
    Vec3i result = { xyz, xyz, xyz };
    return result;
}

Vec4i new_vec4i(int x, int y, int z, int w) {
    Vec4i result = { x, y, z, w };
    return result;
}

Vec4i new_vec4i(int xyzw) {
    Vec4i result = { xyzw, xyzw, xyzw, xyzw };
    return result;
}

bool floats_equal(float a, float b) {
    return fabsf(a - b) < COMPARE_FLOAT_PRECISION;
}

bool vec2f_equal(Vec2f a, Vec2f b) {
    return (fabsf(a.x - b.x) < COMPARE_FLOAT_PRECISION) 
        && (fabsf(a.y - b.y) < COMPARE_FLOAT_PRECISION);
}

bool vec3f_equal(Vec3f a, Vec3f b) {
    return (fabsf(a.x - b.x) < COMPARE_FLOAT_PRECISION) 
        && (fabsf(a.y - b.y) < COMPARE_FLOAT_PRECISION)
        && (fabsf(a.z - b.z) < COMPARE_FLOAT_PRECISION);
}

bool vec4f_equal(Vec4f a, Vec4f b) {
    return (fabsf(a.x - b.x) < COMPARE_FLOAT_PRECISION) 
        && (fabsf(a.y - b.y) < COMPARE_FLOAT_PRECISION)
        && (fabsf(a.z - b.z) < COMPARE_FLOAT_PRECISION)
        && (fabsf(a.w - b.w) < COMPARE_FLOAT_PRECISION);
}

bool vec2i_equal(Vec2i a, Vec2i b) {
    return (fabsf(a.x - b.x) < COMPARE_FLOAT_PRECISION) 
        && (fabsf(a.y - b.y) < COMPARE_FLOAT_PRECISION);
}

bool vec3i_equal(Vec3i a, Vec3i b) {
    return (fabsf(a.x - b.x) < COMPARE_FLOAT_PRECISION) 
        && (fabsf(a.y - b.y) < COMPARE_FLOAT_PRECISION)
        && (fabsf(a.z - b.z) < COMPARE_FLOAT_PRECISION);
}

bool vec4i_equal(Vec4i a, Vec4i b) {
    return (fabsf(a.x - b.x) < COMPARE_FLOAT_PRECISION) 
        && (fabsf(a.y - b.y) < COMPARE_FLOAT_PRECISION)
        && (fabsf(a.z - b.z) < COMPARE_FLOAT_PRECISION)
        && (fabsf(a.w - b.w) < COMPARE_FLOAT_PRECISION);
}

Vec3f vec3f_from_vec4f(Vec4f vec4f) {
    Vec3f result = {
        vec4f.x,
        vec4f.y,
        vec4f.z
    };
    return result;
}

Vec4f vec4f_from_vec3f(Vec3f vec3f) {
    Vec4f result = {
        vec3f.x,
        vec3f.y,
        vec3f.z,
        0.0f
    };
    return result;
}

Triangle2D get_tri(float x0, float y0, float x1, float y1, float x2, float y2) {
    Triangle2D result = { 
        x0, y0,
        x1, y1,
        x2, y2
    };
    return result;
}

Triangle2D get_tri(Point2D p0, Point2D p1, Point2D p2) {
    Triangle2D result = { 
        p0.x, p0.y,
        p1.x, p1.y,
        p2.x, p2.y
    };
    return result;
}

Rectangle2D get_rect(float x, float y, float width, float height) {
    Rectangle2D result = {
        x,
        y,
        width,
        height
    };
    return result;
}

Rectangle2Di get_recti(int x, int y, int width, int height) {
    Rectangle2Di result = {
        x,
        y,
        width,
        height
    };
    return result;
}

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float clamp(float min, float value, float max) {
    if (value < min)  return min;
    if (value > max)  return max;
    return value;
}