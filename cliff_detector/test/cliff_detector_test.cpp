#include <cliff_detector/cliff_detector.h>

#include <iostream>

#include <gtest/gtest.h>

class CliffDetectorTest : public ::testing::Test {
 public:
  sensor_msgs::ImagePtr depth_msg;
  sensor_msgs::CameraInfoPtr info_msg;
  cliff_detector::CliffDetector detector;

  unsigned img_height { 480 };
  unsigned img_width { 640 };
  unsigned scan_height { 420 };

  CliffDetectorTest() {
    setDefaultInfoMsg();

    // Configuration
    detector.setDepthImgStepCol(8);
    detector.setDepthImgStepRow(8);
    detector.setBlockSize(16);
    detector.setRangeLimits(0.5, 5.0);
    detector.setSensorMountHeight(0.5);
    detector.setSensorTiltAngle(10);
  }

  void setDefaultInfoMsg() {
    info_msg.reset(new sensor_msgs::CameraInfo);
    info_msg->header.frame_id = "depth_frame";
    info_msg->header.seq = 100;
    info_msg->height = img_height;
    info_msg->width = img_width;
    info_msg->distortion_model = "plumb_bob";
    info_msg->D.resize(5);
    info_msg->K[0] = 570;
    info_msg->K[2] = 314;
    info_msg->K[4] = 570;
    info_msg->K[5] = 239;
    info_msg->K[8] = 1.0;
    info_msg->R[0] = 1.0;
    info_msg->R[4] = 1.0;
    info_msg->R[8] = 1.0;
    info_msg->P[0] = 570;
    info_msg->P[2] = 314;
    info_msg->P[5] = 570;
    info_msg->P[6] = 235;
    info_msg->P[10] = 1.0;
  }

  template<typename T>
  void setDefaultDepthMsg(T value) {
    depth_msg.reset(new sensor_msgs::Image);
    depth_msg->header.frame_id = "depth_frame";
    depth_msg->header.seq = 100;
    depth_msg->header.stamp.fromSec(10.0);
    depth_msg->height = img_height;
    depth_msg->width = img_width;
    depth_msg->is_bigendian = false;
    depth_msg->step = depth_msg->width * sizeof(T);

    if (typeid(T) == typeid(uint16_t)) {
      depth_msg->encoding = sensor_msgs::image_encodings::TYPE_16UC1;
    }
    else if (typeid(T) == typeid(float)) {
      depth_msg->encoding = sensor_msgs::image_encodings::TYPE_32FC1;
    }

    depth_msg->data.resize(depth_msg->width * depth_msg->height * sizeof(T));
    T* depth_row = reinterpret_cast<T*>(&depth_msg->data[0]);
    for (size_t i = 0; i < depth_msg->width * depth_msg->height; ++i) {
        depth_row[i] = value;
    }
  }
};

TEST_F(CliffDetectorTest, encodingSupport)
{
  setDefaultDepthMsg<uint16_t>(1);
  detector.detectCliff(depth_msg, info_msg);

  setDefaultDepthMsg<float>(1);
  detector.detectCliff(depth_msg, info_msg);
}

TEST_F(CliffDetectorTest, unsupportedEncoding)
{
  setDefaultDepthMsg<uint16_t>(1);
  depth_msg->encoding = sensor_msgs::image_encodings::MONO16;
  EXPECT_ANY_THROW(detector.detectCliff(depth_msg, info_msg));
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
