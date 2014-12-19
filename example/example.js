var FastImage = require('../build/Release/FastImage').FastImage;

try {
    var obj = new FastImage("./orig.jpg");

    console.log(obj.getPath() );
    console.log(obj.getWidth() );
    console.log(obj.getHeight() );

    obj.resize(300, 300);
    obj.save("./resised.jpg");
    obj.save("./resised-poor.jpg", [FastImage.JPEG_QUALITY, 5]);
} catch (e) {
    console.log(e.message)
}
