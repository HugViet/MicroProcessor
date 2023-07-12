#ifndef MQ135_H
#define MQ135_H

/// The load resistance on the board
#define RLOAD 10.0 // / Điện trở tải trên bo mạch
/// Calibration resistance at atmospheric CO2 level
#define RZERO 76.63 // / Điện trở hiệu chuẩn ở mức CO2 trong khí quyển
/// Parameters for calculating ppm of CO2 from sensor resistance
#define PARA 116.6020682// / Thông số tính toán ppm CO2 từ điện trở cảm biến
#define PARB 2.769034857

/// Parameters to model temperature and humidity dependence // / Các tham số để mô hình hóa sự phụ thuộc nhiệt độ và độ ẩm
#define CORA 0.00035
#define CORB 0.02718
#define CORC 1.39538
#define CORD 0.0018

/// Atmospheric CO2 level for calibration purposes // / Mức CO2 trong khí quyển cho mục đích hiệu chuẩn
#define ATMOCO2 397.13

float getCorrectionFactor(float t, float h);
float getResistance(int val) ;
float getCorrectedResistance(float t, float h,int val);
float getPPM(int val);
float getCorrectedPPM(float t, float h,int val);
float getRZero(int val);
float getCorrectedRZero(float t, float h,int val);

#endif
