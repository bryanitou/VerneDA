import numpy as np
import math


# This function was extracted from: https://programming-surgeon.com/en/euler-angle-python-en/
def rotation_matrix(theta1, theta2, theta3, order='xyz', unit: str = "deg"):
    """
    input
        theta1, theta2, theta3 = rotation angles in rotation order (degrees)
        oreder = rotation order of x,y,z　e.g. XZY rotation -- 'xzy'
    output
        3x3 rotation matrix (numpy array)
    """
    # Local auxilary variable
    matrix = None

    # Pre-compute all the angles
    c1 = np.cos(theta1 * np.pi / 180.0 if unit == "deg" else theta1)
    s1 = np.sin(theta1 * np.pi / 180.0 if unit == "deg" else theta1)
    c2 = np.cos(theta2 * np.pi / 180.0 if unit == "deg" else theta2)
    s2 = np.sin(theta2 * np.pi / 180.0 if unit == "deg" else theta2)
    c3 = np.cos(theta3 * np.pi / 180.0 if unit == "deg" else theta3)
    s3 = np.sin(theta3 * np.pi / 180.0 if unit == "deg" else theta3)

    # Switch case rotation to be done
    if order == 'xzx':
        matrix = np.array([[c2, -c3 * s2, s2 * s3],
                           [c1 * s2, c1 * c2 * c3 - s1 * s3, -c3 * s1 - c1 * c2 * s3],
                           [s1 * s2, c1 * s3 + c2 * c3 * s1, c1 * c3 - c2 * s1 * s3]])
    elif order == 'xyx':
        matrix = np.array([[c2, s2 * s3, c3 * s2],
                           [s1 * s2, c1 * c3 - c2 * s1 * s3, -c1 * s3 - c2 * c3 * s1],
                           [-c1 * s2, c3 * s1 + c1 * c2 * s3, c1 * c2 * c3 - s1 * s3]])
    elif order == 'yxy':
        matrix = np.array([[c1 * c3 - c2 * s1 * s3, s1 * s2, c1 * s3 + c2 * c3 * s1],
                           [s2 * s3, c2, -c3 * s2],
                           [-c3 * s1 - c1 * c2 * s3, c1 * s2, c1 * c2 * c3 - s1 * s3]])
    elif order == 'yzy':
        matrix = np.array([[c1 * c2 * c3 - s1 * s3, -c1 * s2, c3 * s1 + c1 * c2 * s3],
                           [c3 * s2, c2, s2 * s3],
                           [-c1 * s3 - c2 * c3 * s1, s1 * s2, c1 * c3 - c2 * s1 * s3]])
    elif order == 'zyz':
        matrix = np.array([[c1 * c2 * c3 - s1 * s3, -c3 * s1 - c1 * c2 * s3, c1 * s2],
                           [c1 * s3 + c2 * c3 * s1, c1 * c3 - c2 * s1 * s3, s1 * s2],
                           [-c3 * s2, s2 * s3, c2]])
    elif order == 'zxz':
        matrix = np.array([[c1 * c3 - c2 * s1 * s3, -c1 * s3 - c2 * c3 * s1, s1 * s2],
                           [c3 * s1 + c1 * c2 * s3, c1 * c2 * c3 - s1 * s3, -c1 * s2],
                           [s2 * s3, c3 * s2, c2]])
    elif order == 'xyz':
        matrix = np.array([[c2 * c3, -c2 * s3, s2],
                           [c1 * s3 + c3 * s1 * s2, c1 * c3 - s1 * s2 * s3, -c2 * s1],
                           [s1 * s3 - c1 * c3 * s2, c3 * s1 + c1 * s2 * s3, c1 * c2]])
    elif order == 'xzy':
        matrix = np.array([[c2 * c3, -s2, c2 * s3],
                           [s1 * s3 + c1 * c3 * s2, c1 * c2, c1 * s2 * s3 - c3 * s1],
                           [c3 * s1 * s2 - c1 * s3, c2 * s1, c1 * c3 + s1 * s2 * s3]])
    elif order == 'yxz':
        matrix = np.array([[c1 * c3 + s1 * s2 * s3, c3 * s1 * s2 - c1 * s3, c2 * s1],
                           [c2 * s3, c2 * c3, -s2],
                           [c1 * s2 * s3 - c3 * s1, c1 * c3 * s2 + s1 * s3, c1 * c2]])
    elif order == 'yzx':
        matrix = np.array([[c1 * c2, s1 * s3 - c1 * c3 * s2, c3 * s1 + c1 * s2 * s3],
                           [s2, c2 * c3, -c2 * s3],
                           [-c2 * s1, c1 * s3 + c3 * s1 * s2, c1 * c3 - s1 * s2 * s3]])
    elif order == 'zyx':
        matrix = np.array([[c1 * c2, c1 * s2 * s3 - c3 * s1, s1 * s3 + c1 * c3 * s2],
                           [c2 * s1, c1 * c3 + s1 * s2 * s3, c3 * s1 * s2 - c1 * s3],
                           [-s2, c2 * s3, c2 * c3]])
    elif order == 'zxy':
        matrix = np.array([[c1 * c3 - s1 * s2 * s3, -c2 * s1, c1 * s3 + c3 * s1 * s2],
                           [c3 * s1 + c1 * s2 * s3, c1 * c2, s1 * s3 - c1 * c3 * s2],
                           [-c2 * s3, s2, c2 * c3]])

    return matrix


def euler_from_quaternion(x, y, z, w):
    """
    Convert a quaternion into euler angles (roll, pitch, yaw)
    roll is rotation around x in radians (counterclockwise)
    pitch is rotation around y in radians (counterclockwise)
    yaw is rotation around z in radians (counterclockwise)
    """
    t0 = +2.0 * (w * x + y * z)
    t1 = +1.0 - 2.0 * (x * x + y * y)
    roll_x = math.atan2(t0, t1)

    t2 = +2.0 * (w * y - z * x)
    t2 = +1.0 if t2 > +1.0 else t2
    t2 = -1.0 if t2 < -1.0 else t2
    pitch_y = math.asin(t2)

    t3 = +2.0 * (w * z + x * y)
    t4 = +1.0 - 2.0 * (y * y + z * z)
    yaw_z = math.atan2(t3, t4)

    return roll_x, pitch_y, yaw_z  # in radians
