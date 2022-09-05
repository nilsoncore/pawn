#ifndef PAWN_MATH_H
#define PAWN_MATH_H
#include <GLM/glm.hpp>

// glm::perspective, glm::lookAt
#include <GLM/gtc/matrix_transform.hpp>

//
// --- Structs ---
//

// [Vectors]
struct Vec2f {
    union {
        float x;
        float width;
    };
    union {
        float y;
        float height;
    };

    bool operator == (const Vec2f &other) {
        return x == other.x && y == other.y;
    }

    const Vec2f& operator + (const Vec2f &other) {
        return Vec2f { x + other.x, y + other.y };
    }

    const Vec2f& operator - (const Vec2f &other) {
        return Vec2f { x - other.x, y - other.y };
    }

    const Vec2f& operator * (const Vec2f &other) {
        return Vec2f { x * other.x, y * other.y };
    }

    const Vec2f& operator / (const Vec2f &other) {
        return Vec2f { x / other.x, y / other.y };
    }

    const Vec2f& operator + (const float k) {
        return Vec2f { x + k, y + k };
    }

    const Vec2f& operator - (const float k) {
        return Vec2f { x - k, y - k };
    }

    const Vec2f& operator * (const float k) {
        return Vec2f { x * k, y * k };
    }

    const Vec2f& operator / (const float k) {
        return Vec2f { x / k, y / k };
    }
};

// XYZ or RGB
struct Vec3f {
    union {
        float x;
        float r;
        float width;
    };
    union {
        float y;
        float g;
        float height;
    };
    union {
        float z;
        float b;
        float depth;
    };

    bool operator == (Vec3f other) {
        return x == other.x && y == other.y && z == other.z;
    }
};

// XYZW or RGBA
struct Vec4f {
    union {
        float x;
        float r;
        float left;
    };
    union {
        float y;
        float g;
        float top;
    };
    union {
        float z;
        float b;
        float right;
    };
    union {
        float w;
        float a;
        float down;
    };

    bool operator == (Vec4f other) {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }
};

struct Vec2i {
    union {
        int x;
        int width;
    };
    union {
        int y;
        int height;
    };

    bool operator == (Vec2i other) {
        return x == other.x && y == other.y;
    }
};

// XYZ or RGB
struct Vec3i {
    union {
        int x;
        int r;
        int width;
    };
    union {
        int y;
        int g;
        int height;
    };
    union {
        int z;
        int b;
        int depth;
    };

    bool operator == (Vec3i other) {
        return x == other.x && y == other.y && z == other.z;
    }
};

// XYZW or RGBA
struct Vec4i {
    union {
        int x;
        int r;
        int left;
    };
    union {
        int y;
        int g;
        int top;
    };
    union {
        int z;
        int b;
        int right;
    };
    union {
        int w;
        int a;
        int left;
    };

    bool operator == (Vec4i other) {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }
};

// [Geometry]
struct Point2D {
    float x;
    float y;
};

struct Triangle2D {
    float x0, y0;
    float x1, y1;
    float x2, y2;
};

struct Rectangle2D {
    float x;
    float y;
    float width;
    float height;
};

struct Rectangle2Di {
    int x;
    int y;
    int width;
    int height;
};

struct Point3D {
    float x;
    float y;
    float z;
};

struct Triangle3D {
    float x0, y0, z0;
    float x1, y1, z1;
    float x2, y2, z2;
};

struct Rectangle3D {
    float x;
    float y;
    float z;
    float width;
    float height;
};

struct Rectangle3Di {
    int x;
    int y;
    int z;
    int width;
    int height;
};

//
// --- Constants ---
//
const float COMPARE_FLOAT_PRECISION = 0.1f;

//
// --- Functions ---
//
Vec2f new_vec2f(float x, float y);
Vec2f new_vec2f(float xy);
Vec3f new_vec3f(float x, float y, float z);
Vec3f new_vec3f(float xyz);
Vec4f new_vec4f(float x, float y, float z, float w);
Vec4f new_vec4f(float xyzw);
Vec2i new_vec2i(int x, int y);
Vec2i new_vec2i(int xy);
Vec3i new_vec3i(int x, int y, int z);
Vec3i new_vec3i(int xyz);
Vec4i new_vec4i(int x, int y, int z, int w);
Vec4i new_vec4i(int xyzw);

bool floats_equal(float a, float b);
bool vec2f_equal(Vec2f a, Vec2f b);
bool vec3f_equal(Vec3f a, Vec3f b);
bool vec4f_equal(Vec4f a, Vec4f b);
bool vec2i_equal(Vec2i a, Vec2i b);
bool vec3i_equal(Vec3i a, Vec3i b);

bool vec4i_equal(Vec4i a, Vec4i b);
Vec3f vec3f_from_vec4f(Vec4f vec4f);
Vec4f vec4f_from_vec3f(Vec3f vec3f);

Triangle2D get_tri(float x0, float y0, float x1, float y1, float x2, float y2);
Triangle2D get_tri(Point2D p0, Point2D p1, Point2D p2);
Rectangle2D get_rect(float x, float y, float width, float height);
Rectangle2Di get_recti(int x, int y, int width, int height);

float lerp(float a, float b, float t);
float clamp(float min, float value, float max);

#endif /* PAWN_MATH_H */
