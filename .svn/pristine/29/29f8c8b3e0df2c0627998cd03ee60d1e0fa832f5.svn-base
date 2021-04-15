#ifndef __CRC16_H__
#define __CRC16_H__


#define CRC16_SMALL   0  // расчет без таблицы
#define CRC16_LARGE   1  // табличный метод (больше по размеру кода)
#define CRC16_MODEL_TYPE    CRC16_LARGE   


#if (CRC16_MODEL_TYPE==CRC16_LARGE)
  extern unsigned short CRC16_large( unsigned char *pBuf, unsigned char ucLength );
  #define CRC16 CRC16_large
#else
  extern unsigned short CRC16_small( unsigned char *pBuf, unsigned char ucLength );
  #define CRC16 CRC16_small
#endif

  
#endif // #ifndef __CRC16_H__

