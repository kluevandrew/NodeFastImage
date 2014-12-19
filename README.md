NodeFastImage
=============

OpenCV Image resize for node.

## Step 0: Install OpenCV

[See official guide](http://docs.opencv.org/doc/tutorials/introduction/table_of_content_introduction/table_of_content_introduction.html)


## Step 1: Installation 

#### Step 1.1: Configure project with node-gyp

``` bash
node-gyp configure
```

#### Step 1.2: Build project with node-gyp

``` bash
node-gyp build
```

#### Step 1.3 Import it in your NodeJS script

``` JavaScript
    var FastImage = require('{PATH TO NodeFastImage DIR}/build/Release/FastImage').FastImage;
```

## Step 2 Have fun!


# Examples

Open file:
``` JavaScript
    var image = new FastImage("{PATH_TO_IMAGE}");
```

Resize image:
``` JavaScript
    var image = new FastImage("{PATH_TO_IMAGE}");
    var keepAspectRatio = false;
    image.resize(width, height, keepAspectRatio);
    
    image.resize(300, 300);
    image.resize(300, 300, keepAspectRatio = true); // Closest available size
    image.resize(300, undefined, keepAspectRatio = true);
    image.resize(undefined, 300, keepAspectRatio = true);
```

Crop image:
``` JavaScript
    var image = new FastImage("{PATH_TO_IMAGE}");
    
    image.crop(x, y, width, height);
```

Save image:
``` JavaScript
    var image = new FastImage("{PATH_TO_IMAGE}");
    
    image.save("{NEW_IMAGE_PATH"); // Save to new file
    image.save(); // Save to old file
```

Get image params:
``` JavaScript
    var image = new FastImage("{PATH_TO_IMAGE}");
    
    image.getPath();  // returns {PATH_TO_IMAGE}
    image.getWidth(); 
    image.getHeight(); 
```

Each method can throws Error!