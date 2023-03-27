#!env/python
# System packages
import math
import os

# Import plotting stuff
import matplotlib.pyplot as plt

# Statistics stuff
from scipy.stats import norm


def get_theta_from_w(w: [float]):
    return [2 * math.acos(w_val) for w_val in w]


def quaternion_histogram(w: [float], x: [float], y: [float], z: [float], unit_str: str = None,
                         output: os.PathLike or str = None):
    # Get theta
    t = get_theta_from_w(w)

    # Do fitting
    mu_t, sigma_t = norm.fit(t)
    mu_w, sigma_w = norm.fit(w)
    mu_x, sigma_x = norm.fit(x)
    mu_y, sigma_y = norm.fit(y)
    mu_z, sigma_z = norm.fit(z)

    # Set the size
    fig, ax = plt.subplots(5, figsize=(16, 9))

    # Plot histogram
    n_t, bins_t, patches_t = ax[0].hist(t, bins=200, density=True, label=r"$theta$")
    n_w, bins_w, patches_w = ax[1].hist(w, bins=200, density=True, label=r"$q_w$")
    n_x, bins_x, patches_x = ax[2].hist(x, bins=200, density=True, label=r"$q_x$")
    n_y, bins_y, patches_y = ax[3].hist(y, bins=200, density=True, label=r"$q_y$")
    n_z, bins_z, patches_z = ax[4].hist(z, bins=200, density=True, label=r"$q_z$")

    # Get best fit
    y_t = norm.pdf(bins_t, mu_t, sigma_t)
    y_w = norm.pdf(bins_w, mu_w, sigma_w)
    y_x = norm.pdf(bins_x, mu_x, sigma_x)
    y_y = norm.pdf(bins_y, mu_y, sigma_y)
    y_z = norm.pdf(bins_z, mu_z, sigma_z)

    ax[0].plot(bins_t, y_t, label=f"Norm({'{:.2e}'.format(mu_t)},{'{:.2e}'.format(sigma_t)})")
    ax[1].plot(bins_w, y_w, label=f"Norm({'{:.2e}'.format(mu_w)},{'{:.2e}'.format(sigma_w)})")
    ax[2].plot(bins_x, y_x, label=f"Norm({'{:.2e}'.format(mu_x)},{'{:.2e}'.format(sigma_x)})")
    ax[3].plot(bins_y, y_y, label=f"Norm({'{:.2e}'.format(mu_y)},{'{:.2e}'.format(sigma_y)})")
    ax[4].plot(bins_z, y_z, label=f"Norm({'{:.2e}'.format(mu_z)},{'{:.2e}'.format(sigma_z)})")

    fig.suptitle(rf"Histogram of $q_i$")

    # Add legends
    ax[0].legend()
    ax[1].legend()
    ax[2].legend()
    ax[3].legend()
    ax[4].legend()

    # Save figure
    plt.savefig(output.replace(".png", "-histogram.png"))

    # Close stuff
    plt.clf()
    plt.cla()
    plt.close(fig)
