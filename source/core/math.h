#pragma once

////////////////////////////////////////////////////////////////////////////////
// F32

f32 to_radians(const f32 degrees)
{
    return degrees * PI / 180.f;
}

f32 to_degrees(const f32 radians)
{
    return radians * (180.f / PI);
}

f32 angular(f32 radians)
{
    f32 result = to_degrees(radians);
    result = result < 0 ? 360 + result : result;
    
    return result;
}

f32 clamp(const f32 v, const f32 min, const f32 max)
{
	if (v < min) return min;	
	if (v > max) return max;

	return v;
}

f32 wrap(const f32 value, const f32 lower, const f32 upper)
{
    f32 rangeZero = upper - lower;

    if (value >= lower && value <= upper)
        return value;

    return fmod(value, rangeZero) + lower;
}

f32 angular_wrap(const f32 v)
{
	if (v >= 360) return v - 360;
	if (v <= -360) return v + 360;
	return v;
}

f32 angular_diff(const f32 angle1, const f32 angle2)
{
    f32 diff = (i32)(angle2 - angle1 + 180) % 360 - 180;
    return diff < -180 ? diff + 360 : diff;
}

// percentage [0..1]
f32 lerp(const f32 start, const f32 target, const f32 percentage)
{
    return start + percentage * (target - start);
}

f32 angular_lerp(const f32 start, const f32 target, const f32 percentage)
{
    f32 startHelper = start;
    f32 targetHelper = target;

    f32 difference = abs(targetHelper - startHelper);
    if (difference > 180)
    {
        // We need to add on to one of the values.
        if (targetHelper > startHelper)
        {
            // We'll add it on to start...
            startHelper += 360.f;
        }
        else
        {
            // Add it on to end.
            targetHelper += 360.f;
        }

        // Interpolate it.
        f32 result = lerp(startHelper, targetHelper, percentage);

        return wrap(result, 0, 360);
    }

    return lerp(startHelper, targetHelper, percentage);
}

////////////////////////////////////////////////////////////////////////////////
// F2

bool f2_equals(vec a, vec b)
{
    return
    a.x == b.x &&
    a.y == b.y;
}

vec f2_round(vec a)
{
	return (vec)
	{
		round(a.x),
		round(a.y)
	};
}

vec f2_add(const vec a, const vec b)
{
    return (vec)
    {
        a.x + b.x,
        a.y + b.y
    };
}

vec f2_sub(const vec a, const vec b)
{
	return (vec)
	{
		a.x - b.x,
		a.y - b.y
	};
}

vec f2_add_f(const vec a, const f32 b)
{
    return (vec)
    {
        a.x + b,
        a.y + b
    };
}

vec f2_sub_f(const vec a, const f32 b)
{
    return (vec)
    {
        a.x - b,
        a.y - b
    };
}

f32 f2_length(const vec a)
{
    return sqrt(a.x*a.x + a.y*a.y);
}

vec f2_normalize(const vec a)
{
    f32 length = f2_length(a);

    if (length == 0) length = 1.0f;

    f32 ilength = 1.0f / length;

    return (vec)
    {
        a.x * ilength,
        a.y * ilength
    };
}


vec f2_mul(const vec a, const vec b)
{
    return (vec)
    {
        a.x * b.x,
        a.y * b.y
    };
}

vec f2_mul_f(const vec a, const f32 b)
{
    return (vec)
    {
        a.x * b,
        a.y * b
    };
}

vec f2_div_f(const vec a, const f32 b)
{
    return (vec)
    {
        a.x / b,
        a.y / b
    };
}


f32 f2_product(vec a, vec b)
{
    return a.x * b.x + a.y * b.y;
}

f32 from_direction(const vec v)
{	
    return angular(atan2(v.x, v.y));
}

void rotate(vec* point, const vec pivot, const f32 angle)
{
    const f32 x =
        (point->x - pivot.x) * cos(angle) -
        (point->y - pivot.y) * sin(angle) +
        pivot.x;

    const f32 y =
        (point->x - pivot.x) * sin(angle) +
        (point->y - pivot.y) * cos(angle) +
        pivot.y;

    point->x = x;
    point->y = y;
}

vec f2_uni(const f32 v)
{
	return (vec) { v, v };
}

vec f2_lerp(const vec start, const vec target, const f32 percentage)
{
    return (vec)
    {
        start.x + percentage * (target.x - start.x),
        start.y + percentage * (target.y - start.y)
    };    
}

vec f2_med(vec a, vec b)
{
    return (vec)
    {
        (a.x + b.x) / 2,
        (a.y + b.y) / 2
    };
}

////////////////////////////////////////////////////////////////////////////////
// F3

bool f3_equals(vec3 a, vec3 b)
{
    return
    a.x == b.x &&
    a.y == b.y &&
    a.z == b.z;
}

vec3 f3_add(vec3 a, vec3 b)
{
    return (vec3)
    {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
    };
}

vec3 f3_sub(vec3 a, vec3 b)
{
    return (vec3)
    {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
    };
}

vec3 f3_med(vec3 a, vec3 b)
{
    return (vec3)
    {
        (a.x + b.x) / 2,
        (a.y + b.y) / 2,
        (a.z + b.z) / 2,
    };
}

vec3 f3_add_f(const vec3 a, const f32 b)
{
    return (vec3)
    {
        a.x + b,
        a.y + b,
        a.y + b
    };
}

vec3 f3_sub_f(const vec3 a, const f32 b)
{
    return (vec3)
    {
        a.x - b,
        a.y - b,
        a.z - b
    };
}

vec3 f3_negative(vec3 a)
{
    return (vec3)
    {
        -a.x,
        -a.y,
        -a.z
    };    
}

vec3 f3_mul(vec3 a, vec3 b)
{
    return (vec3)
    {
        a.x * b.x,
        a.y * b.y,
        a.z * b.z
    };
}

vec3 f3_mul_f(vec3 a, f32 b)
{
    return (vec3)
    {
        a.x * b,
        a.y * b,
        a.z * b
    };
}

vec3 f3_div_f(vec3 a, f32 b)
{
    return (vec3)
    {
        a.x / b,
        a.y / b,
        a.z / b
    };
}

f32 f3_dot(vec3 a, vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 f3_div(vec3 a, vec3 b)
{
    return (vec3)
    {
        a.x / b.x,
        a.y / b.y,
        a.z / b.z,
    };
}

vec3 f3_floor(vec3 a)
{
    return (vec3)
    {
        a.x = (i32)a.x,
        a.y = (i32)a.y,
        a.z = (i32)a.z,
    };
}

vec3 f3_ceil(vec3 a)
{
    return f3_add(f3_floor(a), F3_ONE);
}

f32 f3_length(vec3 a)
{
    return (f32)sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

vec3 f3_normalize(vec3 a)
{
    f32 m = f3_length(a);
   	if (m > 0)
    	return f3_div_f(a, m);
    else
    	return F3_ZERO;
}

vec3 f3_cross(vec3 a, vec3 b)
{
    return (vec3)
    {
		(a.y * b.z) - (a.z * b.y),
		(a.z * b.x) - (a.x * b.z),
		(a.x * b.y) - (a.y * b.x)
	};
}

vec3 f3_lerp(const vec3 start, const vec3 target, const f32 percentage)
{
	return (vec3)
	{
		start.x + percentage * (target.x - start.x),
		start.y + percentage * (target.y - start.y),
		start.z + percentage * (target.z - start.z)
	};    
}

vec3 f3_uni(const f32 v)
{
	return (vec3) { v, v, v };
}

vec3 f2_addz(const vec v)
{
	return (vec3) { v.x, v.y, 0 };
}

vec f3_dropz(const vec3 v)
{
    return (vec) { v.x, v.y };
}

////////////////////////////////////////////////////////////////////////////////
// F4

bool f4_equals(rect a, rect b)
{
    return
    a.x == b.x &&
    a.y == b.y &&
    a.w == b.w &&
    a.w == b.h;
}

rect rect_intersection(rect r, rect s)
{
    f32 x1 = fmax(r.x, s.x);
    f32 x2 = fmin(r.x + r.w, s.x + s.w);
    f32 y1 = fmax(r.y, s.y);
    f32 y2 = fmin(r.y + r.h, s.y + s.h);

    if (x1 < x2 && y1 < y2)
        return (rect) { x1, y1, x2-x1, y2-y1 };

    return F4_ZERO;
}

bool vector_in_rect(const vec v, const rect r)
{
    return
        r.x <= v.x && r.x + r.w >= v.x &&
        r.y <= v.y && r.y + r.h >= v.y;
}

bool vector_in_area(const vec v, const vec o, const size a)
{
    return
        o.x <= v.x && o.x + a.w >= v.x &&
        o.y <= v.y && o.y + a.h >= v.y;

/* centered version    
    return
        o.x - a.w / 2 <= v.x && o.x + a.w / 2 >= v.x &&
        o.y - a.h / 2 <= v.y && o.y + a.h / 2 >= v.y;
*/        
}

////////////////////////////////////////////////////////////////////////////////
// MAT
// https://github.com/orangeduck/Corange/blob/master/src/cengine.c
// https://github.com/arkanis/single-header-file-c-libs/blob/master/math_3d.h

mat mat_scale(vec3 scale)
{
	mat m = MAT_ONE;

	m.xx = scale.x;
	m.yy = scale.y;
	m.zz = scale.z;

	return m;
}

mat mat_translate(vec3 v)
{
	mat m = MAT_ONE;

  	m.xw = v.x;
  	m.yw = v.y;
  	m.zw = v.z;

	return m;
}

mat mat_transpose(mat m)
{
    return (mat)
    {
        m.xx, m.yx, m.zx, m.wx,
        m.xy, m.yy, m.zy, m.wy,
        m.xz, m.yz, m.zz, m.wz,
        m.xw, m.yw, m.zw, m.ww
    };
}

mat mat_mul(mat m1, mat m2)
{
    mat m;
	    
	m.xx = (m1.xx * m2.xx) + (m1.xy * m2.yx) + (m1.xz * m2.zx) + (m1.xw * m2.wx);
	m.xy = (m1.xx * m2.xy) + (m1.xy * m2.yy) + (m1.xz * m2.zy) + (m1.xw * m2.wy);
	m.xz = (m1.xx * m2.xz) + (m1.xy * m2.yz) + (m1.xz * m2.zz) + (m1.xw * m2.wz);
	m.xw = (m1.xx * m2.xw) + (m1.xy * m2.yw) + (m1.xz * m2.zw) + (m1.xw * m2.ww);

	m.yx = (m1.yx * m2.xx) + (m1.yy * m2.yx) + (m1.yz * m2.zx) + (m1.yw * m2.wx);
	m.yy = (m1.yx * m2.xy) + (m1.yy * m2.yy) + (m1.yz * m2.zy) + (m1.yw * m2.wy);
	m.yz = (m1.yx * m2.xz) + (m1.yy * m2.yz) + (m1.yz * m2.zz) + (m1.yw * m2.wz);
	m.yw = (m1.yx * m2.xw) + (m1.yy * m2.yw) + (m1.yz * m2.zw) + (m1.yw * m2.ww);

	m.zx = (m1.zx * m2.xx) + (m1.zy * m2.yx) + (m1.zz * m2.zx) + (m1.zw * m2.wx);
	m.zy = (m1.zx * m2.xy) + (m1.zy * m2.yy) + (m1.zz * m2.zy) + (m1.zw * m2.wy);
	m.zz = (m1.zx * m2.xz) + (m1.zy * m2.yz) + (m1.zz * m2.zz) + (m1.zw * m2.wz);
	m.zw = (m1.zx * m2.xw) + (m1.zy * m2.yw) + (m1.zz * m2.zw) + (m1.zw * m2.ww);

	m.wx = (m1.wx * m2.xx) + (m1.wy * m2.yx) + (m1.wz * m2.zx) + (m1.ww * m2.wx);
	m.wy = (m1.wx * m2.xy) + (m1.wy * m2.yy) + (m1.wz * m2.zy) + (m1.ww * m2.wy);
	m.wz = (m1.wx * m2.xz) + (m1.wy * m2.yz) + (m1.wz * m2.zz) + (m1.ww * m2.wz);
	m.ww = (m1.wx * m2.xw) + (m1.wy * m2.yw) + (m1.wz * m2.zw) + (m1.ww * m2.ww);

    return m;
}

mat mat_invert(mat m)
{
	// Create shorthands to access matrix members
	f32 m00 = m.xx,  m10 = m.xy,  m20 = m.xz,  m30 = m.xw;
	f32 m01 = m.yx,  m11 = m.yy,  m21 = m.yz,  m31 = m.yw;
	f32 m02 = m.zx,  m12 = m.zy,  m22 = m.zz,  m32 = m.zw;
	
	// Invert 3x3 part of the 4x4 matrix that contains the rotation, etc.
	// That part is called R from here on.
	
	// Calculate cofactor matrix of R
	f32 c00 =   m11*m22 - m12*m21,   c10 = -(m01*m22 - m02*m21),  c20 =   m01*m12 - m02*m11;
	f32 c01 = -(m10*m22 - m12*m20),  c11 =   m00*m22 - m02*m20,   c21 = -(m00*m12 - m02*m10);
	f32 c02 =   m10*m21 - m11*m20,   c12 = -(m00*m21 - m01*m20),  c22 =   m00*m11 - m01*m10;
	
	// Caclculate the determinant by using the already calculated determinants
	// in the cofactor matrix.
	// Second sign is already minus from the cofactor matrix.
	f32 det = m00*c00 + m10*c10 + m20 * c20;
	if (fabsf(det) < 0.00001)
		return MAT_ONE;
	
	// Calcuate inverse of R by dividing the transposed cofactor matrix by the
	// determinant.
	f32 i00 = c00 / det,  i10 = c01 / det,  i20 = c02 / det;
	f32 i01 = c10 / det,  i11 = c11 / det,  i21 = c12 / det;
	f32 i02 = c20 / det,  i12 = c21 / det,  i22 = c22 / det;

	// Combine the inverted R with the inverted translation
	return (mat)
	{
		i00, i10, i20,  -(i00*m30 + i10*m31 + i20*m32),
		i01, i11, i21,  -(i01*m30 + i11*m31 + i21*m32),
		i02, i12, i22,  -(i02*m30 + i12*m31 + i22*m32),
		0,   0,   0,      1
	};
}

mat mat_look(vec3 pov, vec3 at, vec3 up)
{
	vec3 zaxis = f3_normalize(f3_sub(at, pov));
	vec3 xaxis = f3_normalize(f3_cross(up, zaxis));
	vec3 yaxis = f3_cross(zaxis, xaxis);

	mat view_matrix = MAT_ONE;

	view_matrix.xx = xaxis.x;
	view_matrix.xy = xaxis.y;
	view_matrix.xz = xaxis.z;

	view_matrix.yx = yaxis.x;
	view_matrix.yy = yaxis.y;
	view_matrix.yz = yaxis.z;

	view_matrix.zx = -zaxis.x;
	view_matrix.zy = -zaxis.y;
	view_matrix.zz = -zaxis.z;

	view_matrix = mat_mul(
		view_matrix, 
		mat_translate(f3_negative(pov)));

	return mat_transpose(view_matrix);
}

mat mat_ortho(f32 left, f32 right, f32 bottom, f32 top,  f32 nnear, f32 nfar) 
{
	mat m = MAT_ONE;

	m.xx = 2 / (right - left);
	m.yy = 2 / (top - bottom);
	m.zz = 1 / (nnear - nfar);

	m.xw = -1 - 2 * left / (right - left);
	m.yw =  1 + 2 * top  / (bottom - top);
	m.zw = nnear / (nnear - nfar);
   
	return m;
}

mat mat_perspective(f32 fov, f32 ratio, f32 nnear, f32 nfar)
{
	/*
    mat m = mat_zero;
    
	f32 right, left, bottom, top;
  
  	right = -(nnear * tan(fov));
  	left = -right;
  
  	top = ratio * nnear * tan(fov);
  	bottom = -top;

	m.xx = (2.0 * nnear) / (right - left);
	m.yy = (2.0 * nnear) / (top - bottom);
	m.xz = (right + left) / (right - left);
	m.yz = (top + bottom) / (top - bottom);
	m.zz = (-nfar - nnear) / (nfar - nnear);
	m.wz = -1.0;
	m.zw = ( -(2.0 * nnear) * nfar) / (nfar - nnear);
    
    */

	/* working
    mat m = mat_zero;

    f32 top = tan(fov / 360.0 * PI) * nnear;
    f32 right = top * ratio;

    f32 left = -right;    
    f32 bottom = -top;   
 
    f32 rl = (right - left);
    f32 tb = (top - bottom);
    f32 fn = (nfar - nnear);

 	m.xx = (nnear * 2.0f) / rl;

    m.yy = (nnear*2.0f) / tb;

    m.zx = (right + left) / rl;
    m.zy = (top + bottom) / tb;
    m.zz = -(nfar + nnear) / fn;
    m.zw = -1.0f;

    m.wz = -(nfar * nnear * 2.0f) / fn;
    */


	/* last version - working
    mat m = MAT_ZERO;

    f32 ffov = to_radians(fov);

    f32 top = nnear * tanf(ffov * 0.5);
    
    f32 right = top * ratio;

    f32 left = -right;    
    f32 bottom = -top;   
 
    f32 rl = (right - left);
    f32 tb = (top - bottom);
    f32 fn = (nfar - nnear);

 	m.xx = (nnear * 2.0f) / rl;

    m.yy = (nnear * 2.0f) / tb;

    m.zx = (right + left) / rl;
    m.zy = (top + bottom) / tb;
    m.zz = -(nfar + nnear) / fn;
    m.zw = -(nfar * nnear * 2.0f) / fn;

    m.wz = -1.0f;

    return m;
    */
    
	
	f32 f = 1.0f / tanf(to_radians(fov) / 2.0f);
	f32 ar = ratio;
	f32 nd = nnear, fd = nfar;
	
	return (mat)
	{
		 f / ar,           0,                0,                0,
		 0,                f,                0,                0,
		 0,                0,               (fd+nd)/(nd-fd),  (2*fd*nd)/(nd-fd),
		 0,                0,               -1,                0
	};    
	
}

mat mat_rotate_x(f32 radians)
{
    mat m = MAT_ONE;
    
    f32 cosres = cosf(radians);
    f32 sinres = sinf(radians);
    
    m.yy = cosres;
    m.yz = -sinres;
    m.zy = sinres;
    m.zz = cosres;
    
    return m;
}

mat mat_rotate_y(f32 radians)
{
    mat m = MAT_ONE;
    
    f32 cosres = cosf(radians);
    f32 sinres = sinf(radians);
    
    m.xx = cosres;
    m.xz = sinres;
    m.zx = -sinres;
    m.zz = cosres;
    
    return m;
}

mat mat_rotate_z(f32 radians)
{
    mat m = MAT_ONE;
    
    f32 cosres = cosf(radians);
    f32 sinres = sinf(radians);
    
    m.xx = cosres;
    m.xy = -sinres;
    m.yx = sinres;
    m.yy = cosres;

    return m;
}

// rotation in degrees and CW (it's defined CCW in graphics3d)
mat mat_rotate_vec(vec3 rotation)
{
    mat m = MAT_ONE;
   
    if (rotation.z != .0f)
        m = mat_mul(m, mat_rotate_z(to_radians(rotation.z)));
    
    if (rotation.y != .0f)    	
        m = mat_mul(m, mat_rotate_y(to_radians(rotation.y)));

    if (rotation.x != .0f)    	
        m = mat_mul(m, mat_rotate_x(to_radians(rotation.x)));
       
    return m;
}

////////////////////////////////////////////////////////////////////////////////
// EASINGS

/*
https://github.com/warrenm/AHEasing/blob/master/AHEasing/easing.c - code
http://easings.net/ - visual representation
*/

// Modeled after the parabola y = x^2
f32 quadratic_ease_in(const f32 p)
{
    return p * p;
}

// Modeled after the parabola y = -x^2 + 2x
f32 quadratic_ease_out(const f32 p)
{
    return -(p * (p - 2));
}

// Modeled after the piecewise quadratic
// y = (1/2)((2x)^2)             ; [0, 0.5)
// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
f32 quadratic_ease_in_out(const f32 p)
{
    if (p < 0.5)
    {
        return 2 * p * p;
    }
    else
    {
        return (-2 * p * p) + (4 * p) - 1;
    }
}

// Modeled after the cubic y = x^3
f32 cubic_ease_in(const f32 p)
{
    return p * p * p;
}

// Modeled after the cubic y = (x - 1)^3 + 1
f32 cubic_ease_out(const f32 p)
{
    f32 f = (p - 1);
    return f * f * f + 1;
}

// Modeled after the piecewise cubic
// y = (1/2)((2x)^3)       ; [0, 0.5)
// y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
f32 cubic_ease_in_out(const f32 p)
{
    if (p < 0.5)
    {
        return 4 * p * p * p;
    }
    else
    {
        f32 f = ((2 * p) - 2);
        return 0.5 * f * f * f + 1;
    }
}

// Modeled after the quartic x^4
f32 quartic_ease_in(const f32 p)
{
    return p * p * p * p;
}

// Modeled after the quartic y = 1 - (x - 1)^4
f32 quartic_ease_out(const f32 p)
{
    f32 f = (p - 1);
    return f * f * f * (1 - p) + 1;
}

// Modeled after the piecewise quartic
// y = (1/2)((2x)^4)        ; [0, 0.5)
// y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
f32 quartic_ease_in_out(const f32 p)
{
    if (p < 0.5)
    {
        return 8 * p * p * p * p;
    }
    else
    {
        f32 f = (p - 1);
        return -8 * f * f * f * f + 1;
    }
}

// Modeled after the quintic y = x^5
f32 quintic_ease_in(const f32 p)
{
    return p * p * p * p * p;
}

// Modeled after the quintic y = (x - 1)^5 + 1
f32 quintic_ease_out(const f32 p)
{
    f32 f = (p - 1);
    return f * f * f * f * f + 1;
}

// Modeled after the piecewise quintic
// y = (1/2)((2x)^5)       ; [0, 0.5)
// y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
f32 quintic_ease_in_out(const f32 p)
{
    if (p < 0.5)
    {
        return 16 * p * p * p * p * p;
    }
    else
    {
        f32 f = ((2 * p) - 2);
        return  0.5 * f * f * f * f * f + 1;
    }
}

// Modeled after quarter-cycle of sine wave
f32 sine_ease_in(const f32 p)
{
    return sin((p - 1) * HALF_PI) + 1;
}

// Modeled after quarter-cycle of sine wave (different phase)
f32 sine_ease_out(const f32 p)
{
    return sin(p * HALF_PI);
}

// Modeled after half sine wave
f32 sine_ease_in_out(const f32 p)
{
    return 0.5 * (1 - cos(p * PI));
}

// Modeled after shifted quadrant IV of unit circle
f32 circular_ease_in(const f32 p)
{
    return 1 - sqrt(1 - (p * p));
}

// Modeled after shifted quadrant II of unit circle
f32 circular_ease_out(const f32 p)
{
    return sqrt((2 - p) * p);
}

// Modeled after the piecewise circular function
// y = (1/2)(1 - sqrt(1 - 4x^2))           ; [0, 0.5)
// y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
f32 circular_ease_in_out(const f32 p)
{
    if (p < 0.5)
    {
        return 0.5 * (1 - sqrt(1 - 4 * (p * p)));
    }
    else
    {
        return 0.5 * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
    }
}

// Modeled after the exponential function y = 2^(10(x - 1))
f32 exponential_ease_in(const f32 p)
{
    return (p == 0.0) ? p : pow(2, 10 * (p - 1));
}

// Modeled after the exponential function y = -2^(-10x) + 1
f32 exponential_ease_out(const f32 p)
{
    return (p == 1.0) ? p : 1 - pow(2, -10 * p);
}

// Modeled after the piecewise exponential
// y = (1/2)2^(10(2x - 1))         ; [0,0.5)
// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
f32 exponential_ease_in_out(const f32 p)
{
    if (p == 0.0 || p == 1.0) return p;

    if (p < 0.5)
    {
        return 0.5 * pow(2, (20 * p) - 10);
    }
    else
    {
        return -0.5 * pow(2, (-20 * p) + 10) + 1;
    }
}

// Modeled after the damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x - 1))
f32 elastic_ease_in(const f32 p)
{
    return sin(13 * HALF_PI * p) * pow(2, 10 * (p - 1));
}

// Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*pow(2, -10x) + 1
f32 elastic_ease_out(const f32 p)
{
    return sin(-13 * HALF_PI * (p + 1)) * pow(2, -10 * p) + 1;
}

// Modeled after the piecewise exponentially-damped sine wave:
// y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))      ; [0,0.5)
// y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
f32 elastic_ease_in_out(const f32 p)
{
    if (p < 0.5)
    {
        return 0.5 * sin(13 * HALF_PI * (2 * p)) * pow(2, 10 * ((2 * p) - 1));
    }
    else
    {
        return 0.5 * (sin(-13 * HALF_PI * ((2 * p - 1) + 1)) * pow(2, -10 * (2 * p - 1)) + 2);
    }
}

// Modeled after the overshooting cubic y = x^3-x*sin(x*pi)
f32 back_ease_in(const f32 p)
{
    return p * p * p - p * sin(p * PI);
}

// Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi))
f32 back_ease_out(const f32 p)
{
    f32 f = (1 - p);
    return 1 - (f * f * f - f * sin(f * PI));
}

// Modeled after the piecewise overshooting cubic function:
// y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))           ; [0, 0.5)
// y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
f32 back_ease_in_out(const f32 p)
{
    if (p < 0.5)
    {
        f32 f = 2 * p;
        return 0.5 * (f * f * f - f * sin(f * PI));
    }
    else
    {
        f32 f = (1 - (2*p - 1));
        return 0.5 * (1 - (f * f * f - f * sin(f * PI))) + 0.5;
    }
}

f32 bounce_ease_out(const f32 p)
{
    if (p < 4/11.0)
    {
        return (121 * p * p)/16.0;
    }
    else if (p < 8/11.0)
    {
        return (363/40.0 * p * p) - (99/10.0 * p) + 17/5.0;
    }
    else if (p < 9/10.0)
    {
        return (4356/361.0 * p * p) - (35442/1805.0 * p) + 16061/1805.0;
    }
    else
    {
        return (54/5.0 * p * p) - (513/25.0 * p) + 268/25.0;
    }
}

f32 bounce_ease_in(const f32 p)
{
    return 1 - bounce_ease_out(1 - p);
}

f32 bounce_ease_in_out(const f32 p)
{
    if (p < 0.5)
    {
        return 0.5 * bounce_ease_in(p*2);
    }
    else
    {
        return 0.5 * bounce_ease_out(p * 2 - 1) + 0.5;
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool circles_intersect(vec a, f32 ra, vec b, f32 rb)
{
    f32 hole = (ra + rb) / 2.0;
    f32 delta_x = a.x - b.x;
    f32 delta_y = a.y - b.y;    
    
    return (hole * hole > delta_x * delta_x + delta_y * delta_y);    
}

vec circle_rebound(vec a, f32 ra, vec b, f32 rb)
{
	// midpoint
	vec midpoint = f2_sub(b, a);
	f32 angle = atan2(midpoint.y, midpoint.x);


	f32 degrees = to_degrees(angle);
    debug("angle %f\n", angle);

	vec vector_a = (vec)
	{
		cos(angle) * PI / 2 * ra / 2,
		sin(angle) * PI / 2 * ra / 2,
	};

	vec vector_b = (vec)
	{
		-cos(angle + 180) * rb,
		-sin(angle + 180) * rb,
	};

    vec extremity_a = f2_add(a, vector_a);
    vec extremity_b = f2_add(b, vector_b);
	
    vec midinterection = f2_sub(extremity_b, extremity_a);

	vec diff = f2_sub(extremity_a, a);

	return diff;
}
