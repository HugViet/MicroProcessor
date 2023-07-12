#include "MQ135.h"
#include <esp_log.h>
#include <stdio.h>
#include<math.h>
// == get temp & hum =============================================
/**************************************************************************/
/*!
@brief  Get the correction factor to correct for temperature and humidity

@param[in] t  The ambient air temperature nhiệt độ
@param[in] h  The relative humidity Độ ẩm 

@return The calculated correction factor
*/
/**************************************************************************/
float getCorrectionFactor(float t, float h) {
  return CORA * t * t - CORB * t + CORC - (h-33.)*CORD;
}

/**************************************************************************/
/*!
@brief  Get the resistance (trở )of the sensor, ie. the measurement value Giá trị đo lường cảm biến 

@return The sensor resistance in kOhm
*/
/**************************************************************************/
float getResistance(int val) {
  return ((1023./(float)val) * 5. - 1.)*RLOAD;
}

/**************************************************************************/
/*!
@brief  Get the resistance of the sensor, ie. the measurement value corrected
        for temp/hum

@param[in] t  The ambient air temperature : nhiệt độ xung quanh 
@param[in] h  The relative humidity độ ẩm tương đối 

@return The corrected sensor resistance kOhm Điện trở cảm biến hiệu chỉnh 
*/
/**************************************************************************/
float getCorrectedResistance(float t, float h,int val) {
  return getResistance(val)/getCorrectionFactor(t, h);
}

/**************************************************************************/
/*!
@brief  Get the ppm of CO2 sensed (assuming only CO2 in the air)

@return The ppm of CO2 in the air lấy chỉ số ppm cảm biến 
*/
/**************************************************************************/
float getPPM(int val) {
  return PARA * pow((getResistance(val)/RZERO), -PARB);
}

/**************************************************************************/
/*!
@brief  Get the ppm of CO2 sensed (assuming only CO2 in the air), corrected
        for temp/hum

@param[in] t  The ambient air temperature
@param[in] h  The relative humidity

@return The ppm of CO2 in the air
*/
/**************************************************************************/
float getCorrectedPPM(float t, float h,int val) { //giá trị đúng 
  return PARA * pow((getCorrectedResistance(t, h,val)/RZERO), -PARB);
}

/**************************************************************************/
/*!
@brief  Get the resistance RZero of the sensor for calibration purposes

@return The sensor resistance RZero in kOhm
*/
/**************************************************************************/
float getRZero(int val) {  //giá trị hiệu chuẩn 
  return getResistance(val) * pow((ATMOCO2/PARA), (1./PARB));
}

/**************************************************************************/
/*!
@brief  Get the corrected resistance RZero of the sensor for calibration
        purposes Dùng điện trở hiệu chuẩn điều chỉnh 

@param[in] t  The ambient air temperature
@param[in] h  The relative humidity

@return The corrected sensor resistance RZero in kOhm
*/
/**************************************************************************/
float getCorrectedRZero(float t, float h,int val) {
  return getCorrectedResistance(t, h,val) * pow((ATMOCO2/PARA), (1./PARB));
}
