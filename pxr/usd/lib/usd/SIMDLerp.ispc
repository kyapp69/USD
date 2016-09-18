typedef unsigned int16 half;

struct quath
{
    half x, y, z, w;
};

struct quatf
{
    float x, y, z, w;
};

struct quatd
{
    double x, y, z, w;
};


export void LerpHalfArray(uniform half dst[], uniform const half src0[], uniform const half src1[], uniform float w, uniform int n)
{
    double iw = 1.0 - w;
    foreach(i=0 ... n) {
        float f1 = half_to_float(src0[i]);
        float f2 = half_to_float(src1[2]);
        float r = f1 * iw + f2 * w;
        dst[i] = float_to_half(r);
    }
}

export void LerpFloatArray(uniform float dst[], uniform const float src0[], uniform const float src1[], uniform float w, uniform int n)
{
    float iw = 1.0 - w;
    foreach(i=0 ... n) {
        dst[i] = src0[i] * iw + src1[i] * w;
    }
}

export void LerpDoubleArray(uniform double dst[], uniform const double src0[], uniform const double src1[], uniform double w, uniform int n)
{
    double iw = 1.0 - w;
    foreach(i=0 ... n) {
        dst[i] = src0[i] * iw + src1[i] * w;
    }
}


// half quaternion

export void SlerpQuathArray(uniform quath dst[], uniform const quath src0[], uniform const quath src1[], uniform float w, uniform int n)
{
    float iw = 1.0 - w;

    foreach(i=0 ... n) {
        quath qh0 = src0[i];
        quath qh1 = src1[i];
        quatf q0 = { half_to_float(qh0.x), half_to_float(qh0.y), half_to_float(qh0.z), half_to_float(qh0.w) };
        quatf q1 = { half_to_float(qh1.x), half_to_float(qh1.y), half_to_float(qh1.z), half_to_float(qh1.w) };

        float cos_theta = (q0.x*q1.x + q0.y*q1.y + q0.z*q1.z) + (q0.w*q1.w);
        float flip = select(cos_theta < 0.0f, -1.0f, 1.0f);
        cos_theta = abs(cos_theta);

        float scale0, scale1;
        if (1.0f - cos_theta > 0.00001f ) {
            float theta = acos(cos_theta);
            float sin_theta = sin(theta);
            scale0 = sin(iw * theta) / sin_theta;
            scale1 = sin(w * theta) / sin_theta;
        } 
        else {
            scale0 = iw;
            scale1 = w;
        }
        scale1 *= flip;

        quath r = {
            float_to_half(q0.x * scale0 + q1.x * scale1),
            float_to_half(q0.y * scale0 + q1.y * scale1),
            float_to_half(q0.z * scale0 + q1.z * scale1),
            float_to_half(q0.w * scale0 + q1.w * scale1),
        };
        dst[i] = r;
    }
}


// float quaternion

export void SlerpQuatfArray(uniform quatf dst[], uniform const quatf src0[], uniform const quatf src1[], uniform float w, uniform int n)
{
    float iw = 1.0 - w;

    foreach(i=0 ... n) {
        quatf q0 = src0[i];
        quatf q1 = src1[i];

        float cos_theta = (q0.x*q1.x + q0.y*q1.y + q0.z*q1.z) + (q0.w*q1.w);
        float flip = select(cos_theta < 0.0f, -1.0f, 1.0f);
        cos_theta = abs(cos_theta);

        float scale0, scale1;
        if (1.0f - cos_theta > 0.00001f ) {
            float theta = acos(cos_theta);
            float sin_theta = sin(theta);
            scale0 = sin(iw * theta) / sin_theta;
            scale1 = sin(w * theta) / sin_theta;
        } 
        else {
            scale0 = iw;
            scale1 = w;
        }
        scale1 *= flip;

        quatf r = {
            q0.x * scale0 + q1.x * scale1,
            q0.y * scale0 + q1.y * scale1,
            q0.z * scale0 + q1.z * scale1,
            q0.w * scale0 + q1.w * scale1,
        };
        dst[i] = r;
    }
}


// double quaternion

export void SlerpQuatdArray(uniform quatd dst[], uniform const quatd src0[], uniform const quatd src1[], uniform double w, uniform int n)
{
    double iw = 1.0 - w;

    foreach(i=0 ... n) {
        quatd q0 = src0[i];
        quatd q1 = src1[i];

        double cos_theta = (q0.x*q1.x + q0.y*q1.y + q0.z*q1.z) + (q0.w*q1.w);
        double flip = select(cos_theta < 0.0, -1.0, 1.0);
        cos_theta = abs(cos_theta);

        double scale0, scale1;
        if (1.0 - cos_theta > 0.00001 ) {
            float theta = acos(cos_theta);
            float sin_theta = sin(theta);
            scale0 = sin(iw * theta) / sin_theta;
            scale1 = sin(w * theta) / sin_theta;
        } 
        else {
            scale0 = iw;
            scale1 = w;
        }
        scale1 *= flip;

        quatd r = {
            q0.x * scale0 + q1.x * scale1,
            q0.y * scale0 + q1.y * scale1,
            q0.z * scale0 + q1.z * scale1,
            q0.w * scale0 + q1.w * scale1,
        };
        dst[i] = r;
    }
}
