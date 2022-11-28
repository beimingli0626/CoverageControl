import sys
import math
import time
import numpy as np
import pyCoverageControl # Main library
from pyCoverageControl import Point2 # for defining points
from pyCoverageControl import PointVector # for defining list of points
from pyCoverageControl import CoverageSystem

# We can visualize the map in python
import matplotlib.pylab as plt
import seaborn as sns
colormap = sns.color_palette("light:b", as_cmap=True)

params_ = pyCoverageControl.Parameters('parameters.yaml')

num_gaussians = 100
num_robots = 40
env = CoverageSystem(params_, num_gaussians, num_robots)
map = env.GetWorldIDF()
robot_positions = env.GetRobotPositions()
print(type(map))
print(type(robot_positions))
plot_pos_x = np.array([])
plot_pos_y = np.array([])
for pos in robot_positions:
    plot_pos_x = np.append(plot_pos_x, pos.x / params_.pResolution)
    plot_pos_y = np.append(plot_pos_y, pos.y / params_.pResolution)

env.ComputeVoronoiCells()
voronoi_cells = env.GetVoronoiCells()
voronoi_edges = env.GetVoronoiEdges()



plt.ion()
fig = plt.figure()
ax = sns.heatmap(map.transpose(), vmax=params_.pNorm, cmap=colormap, square=True)
ax.invert_yaxis()
nrow, ncol = map.shape
# septicks = 5 ** (math.floor(math.log(nrow, 5)) - 1)
septicks = 10 ** (math.floor(math.log(nrow, 10)))
plt.xticks(np.arange(0, nrow, septicks), np.arange(0, nrow, septicks))
plt.yticks(np.arange(0, ncol, septicks), np.arange(0, ncol, septicks))

plot_robots, = ax.plot(plot_pos_x, plot_pos_y, 'go')
print(plot_robots)
centroid_x = np.array([])
centroid_y = np.array([])
plot_cells = []
for vcell in voronoi_cells:
    cell = vcell.cell
    plot_pt_x = np.array([])
    plot_pt_y = np.array([])
    for pt in cell:
        plot_pt_x = np.append(plot_pt_x, pt.x / params_.pResolution)
        plot_pt_y = np.append(plot_pt_y, pt.y / params_.pResolution)
    plot_pt_x = np.append(plot_pt_x, plot_pt_x[0])
    plot_pt_y = np.append(plot_pt_y, plot_pt_y[0])
    plot_cell, = ax.plot(plot_pt_x, plot_pt_y, 'r')
    plot_cells.append(plot_cell)
    centroid_x = np.append(centroid_x, vcell.centroid.x)
    centroid_y = np.append(centroid_y, vcell.centroid.y)
# for edge in voronoi_edges:
#     ax.plot([edge.x1, edge.x2], [edge.y1, edge.y2], 'r')
plot_centroids, = ax.plot(centroid_x, centroid_y, 'r+')
# plt.show()
fig.canvas.draw()
fig.canvas.flush_events()

# for step in range(0, params_.pEpisodeSteps):
#     print(step)
#     env.StepLloyd()
prev_robot_pos = robot_positions
for step in range(0, params_.pEpisodeSteps):
    print(step)
    env.StepLloyd()
    voronoi_cells = env.GetVoronoiCells()
    robot_positions = env.GetRobotPositions()
    for i in range(0, num_robots):
        plot_pos_x[i] =  robot_positions[i].x / params_.pResolution
        plot_pos_y[i] =  robot_positions[i].y / params_.pResolution
        plot_pt_x = np.array([])
        plot_pt_y = np.array([])
        for pt in voronoi_cells[i].cell:
            plot_pt_x = np.append(plot_pt_x, pt.x / params_.pResolution)
            plot_pt_y = np.append(plot_pt_y, pt.y / params_.pResolution)
        plot_pt_x = np.append(plot_pt_x, plot_pt_x[0])
        plot_pt_y = np.append(plot_pt_y, plot_pt_y[0])

        plot_cells[i].set_xdata(plot_pt_x)
        plot_cells[i].set_ydata(plot_pt_y)

        # ax.plot(plot_pt_x, plot_pt_y, 'r')
        centroid_x[i] = voronoi_cells[i].centroid.x
        centroid_y[i] = voronoi_cells[i].centroid.y
    # plot_centroids = ax.plot(centroid_x, centroid_y, 'r+')
    prev_robot_pos = robot_positions
    plot_robots.set_xdata(plot_pos_x)
    plot_robots.set_ydata(plot_pos_y)
    plot_centroids.set_xdata(centroid_x)
    plot_centroids.set_ydata(centroid_y)

    fig.canvas.draw()
    fig.canvas.flush_events()

