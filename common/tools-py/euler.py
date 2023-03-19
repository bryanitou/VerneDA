import numpy as np


# This function was extracted from: https://programming-surgeon.com/en/euler-angle-python-en/
def rotation_matrix(theta1, theta2, theta3, order='xyz'):
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
    c1 = np.cos(theta1 * np.pi / 180.0)
    s1 = np.sin(theta1 * np.pi / 180.0)
    c2 = np.cos(theta2 * np.pi / 180.0)
    s2 = np.sin(theta2 * np.pi / 180.0)
    c3 = np.cos(theta3 * np.pi / 180.0)
    s3 = np.sin(theta3 * np.pi / 180.0)

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
