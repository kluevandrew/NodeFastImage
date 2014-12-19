{
  "targets": [
    {
      "target_name": "FastImage",
      "sources": [ "NodeFastImage.cc", "FastImage.cc"],
      "cflags!": ["-fno-rtti"],
      "cflags_cc!": ["-fno-rtti"],
      "cflags": ["-fexceptions"],
      "cflags_cc": ["-fexceptions"],
      "ldflags": [
        "-lopencv_core",
        "-lopencv_calib3d",
        "-lopencv_imgproc",
        "-lopencv_contrib",
        "-lopencv_legacy",
        "-lopencv_highgui"
      ]
    }
  ]
}