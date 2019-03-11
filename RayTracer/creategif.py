import imageio
import os
images = []
filenames = os.listdir()
filenames.sort()
for filename in filenames:
      print (filename)
      if filename[-3:]=="png":
            images.append(imageio.imread(filename))
imageio.mimsave('animation.gif', images)
