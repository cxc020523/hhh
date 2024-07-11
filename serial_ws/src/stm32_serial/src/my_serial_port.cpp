#include<ros/ros.h>
#include<serial/serial.h>
#include<std_msgs/String.h>

#include "serial_port.h"

using namespace std;

const uint16_t crc_table[16]=
{
  //CRC-16
  0x0000,0x1021,0x2042,0x3063,0x4084,0x50A5,0x60C6,0x70E7,
  0x8108,0x9129,0xA14A,0xB16B,0xC18C,0xD1AD,0xE1CE,0xF1EF
};


uint16_t Cal_CRC(const uint8_t *ptr, uint32_t length)
{
  static uint16_t crc;
  static uint8_t dat;

  crc=0xffff;
  if(length > 0)
  {
    while(length--)
    {
      dat=((uint8_t)(crc >> 12));
      crc <<= 4;
      crc ^= crc_table[dat ^ ((*ptr) >> 4)];

      dat=((uint8_t)(crc >> 12));
      crc <<= 4;
      crc ^= crc_table[dat ^ (*ptr & 0x0f)];
      ptr++;
    }
  }
  return crc;
}

void Write_Data(uint32_t cmd, uint8_t *sData, uint16_t dataLen)
{
    memcpy(sData+11, sData, dataLen);
    dataLen += 0x0D;

    sData[0] = HEAD_H;
    sData[1] = HEAD_L;
    sData[2] = (uint8_t)((dataLen >> 8) & 0x00FF);
    sData[3] = (uint8_t)(dataLen & 0x00FF);
    sData[4] = HW_ADDR;
    sData[5] = (uint8_t)((Board_Type >> 8) & 0x00FF);
    sData[6] = (uint8_t)(Board_Type & 0x00FF);
    sData[7] = (uint8_t)((cmd >> 24) & 0x000000FF);
    sData[8] = (uint8_t)((cmd >> 16) & 0x000000FF);
    sData[9] = (uint8_t)((cmd >> 8) & 0x000000FF);
    sData[10] = (uint8_t)(cmd & 0x000000FF);

    sData[dataLen-2] = (uint8_t)((Cal_CRC(sData, dataLen-2)>>8)&0x00FF);
    sData[dataLen-1] = (uint8_t)(Cal_CRC(sData, dataLen-2)&0x00FF);

    ROS_INFO("write data ok!");
}


int main(int argc, char** argv)
{
    ros::init(argc, argv,"my_serial_port");
    ros::NodeHandle nh;
    
    //创建一个serial类
    serial::Serial ser;

    //初始化串口相关设置
    ser.setPort("/dev/ttyUSB0");         //设置打开的串口名称
    ser.setBaudrate(115200);                //设置串口的波特率
    serial::Timeout to = serial::Timeout::simpleTimeout(1000);           //创建timeout
    ser.setTimeout(to);                           //设置串口的timeout

    //打开串口
    try
    {
        ser.open();         //打开串口
    }
    catch(const std::exception& e)
    {
        ROS_ERROR_STREAM("Unable to open port ");           //打开串口失败，打印信息
        return -1;
    }

    //判断串口是否成功打开
    if( ser.isOpen() )
    { 
        ROS_INFO_STREAM("Serial Port initialized. \n");         //成功打开串口，打印信息
    

        //buf 为发送数据
        //result 为接收数据
        uint8_t buf[128] = {0};
        uint8_t rData[128]= {0};

        ROS_INFO_STREAM("data start. \n");
        int func(0);
        
        ros::Rate loop_rate(100);
        cout << "Please input function number:" << endl;
        
        ser.flushInput();
        
        while( ros::ok() )
        {
            //从键盘中读取键入数据
            cout << "Your function number is: ";
            cin >> func;
            uint16_t len = 2;
            
            if(func == 0)
            {
                //ros::shutdown;
                ser.close();
                break;
            }
            else
            {
                switch (func)
                {                    
                    case 1:    
                        buf[0] = 0x01;
                        buf[1] = 0x00;        
                        break;
                    case 2:     
                        buf[0] = 0x00;
                        buf[1] = 0x01;        
                        break;

                    case 3:     
                        buf[0] = 0x01;
                        buf[1] = 0x01;        
                        break;
                    case 4:     
                        buf[0] = 0x00;
                        buf[1] = 0x00;        
                        break;
                    default:    ROS_ERROR_STREAM("No this function number!!!");     break;
                }
                //串口写数据
                Write_Data(UART_GPIO_CONTROL, buf, len);
                ser.write(buf, len);
                //cout << " the data write to serial is :  " << int(buf[0]) << endl;
                
                loop_rate.sleep();
                //串口读数据
                size_t recieve_n = ser.available();
                int n;
                
                if(recieve_n != 0)
                {
                    n = ser.read(rData, recieve_n);        
                    ROS_INFO("read data : %2x.%2x", rData[11], rData[12]);
                }
                
                loop_rate.sleep();
                cout << endl;
            }
        }
    }
    else
        ROS_INFO_STREAM("Serial Port not open. \n");         //打开串口失败，打印信息

    return 0;
}

