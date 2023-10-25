/**
 * QUATERNION TEMPLATE FILE
 */

template <typename T> std::vector<T> quaternion::quaternion2euler_DACE(T w, T x, T y, T z)
{
    // Auxiliary variable
    T num, den, token;
    T roll, pitch, yaw;

    // Roll
    num = +2.0 * (w * x + y * z);
    den = +1.0 - 2.0 * (x * x + y * y);
    roll = DACE::atan2(num, den);

    // Pitch
    num = DACE::sqrt(1 + 2 * (w * y - x * z));
    token = (1 - 2 * (w * y - x * z));
    // token = token > DACE::DA(0.0) ? token : 0.0;
    den = DACE::sqrt(token);
    pitch = 2 * DACE::atan2(num, den) - M_PI / 2;

    // Yaw
    num = +2.0 * (w * z + x * y);
    den = +1.0 - 2.0 * (y * y + z * z);
    yaw = DACE::atan2(num, den);


    // Return Euler angles
    return DACE::AlgebraicVector<T> {roll, pitch, yaw};
}

template <typename T> std::vector<T> quaternion::quaternion2euler_NORMAL(T w, T x, T y, T z)
{
    // Auxiliary variable
    T num, den, token;
    T roll, pitch, yaw;

    // Roll
    num = +2.0 * (w * x + y * z);
    den = +1.0 - 2.0 * (x * x + y * y);
    roll = atan2(num, den);

    // Pitch
    num = sqrt(1 + 2 * (w * y - x * z));
    token = (1 - 2 * (w * y - x * z));
    token > 0.0 ? token : 0.0;
    den = sqrt(token);
    pitch = 2 * atan2(num, den) - M_PI / 2;

    // Yaw
    num = +2.0 * (w * z + x * y);
    den = +1.0 - 2.0 * (y * y + z * z);
    yaw = atan2(num, den);


    // Return Euler angles
    return std::vector<T> {roll, pitch, yaw};
}