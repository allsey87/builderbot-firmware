#ifndef PACKET_CONTROL_INTERFACE_H
#define PACKET_CONTROL_INTERFACE_H

#include <huart_controller.h>

#define RX_COMMAND_BUFFER_LENGTH 32
#define TX_COMMAND_BUFFER_LENGTH 32

#define PREAMBLE1  0xF0
#define PREAMBLE2  0xCA
#define POSTAMBLE1 0x53
#define POSTAMBLE2 0x0F

#define PREAMBLE_SIZE 2
#define TYPE_FIELD_SIZE 1
#define DATA_LENGTH_FIELD_SIZE 1
#define CHECKSUM_FIELD_SIZE 1
#define POSTAMBLE_SIZE 2

#define NON_DATA_SIZE (PREAMBLE_SIZE + TYPE_FIELD_SIZE + DATA_LENGTH_FIELD_SIZE + \
                       CHECKSUM_FIELD_SIZE + POSTAMBLE_SIZE)

#define TYPE_OFFSET 2
#define DATA_LENGTH_OFFSET 3
#define DATA_START_OFFSET 4
#define CHECKSUM_OFFSET -3

class CPacketControlInterface {

public:
   enum class EState {
      SRCH_PREAMBLE1,
      SRCH_PREAMBLE2,
      SRCH_POSTAMBLE1,
      SRCH_POSTAMBLE2,
      RECV_COMMAND,
      BUF_OVERFLOW,
   }; 

   class CPacket {
   public:
      
      enum class EType : uint8_t {
         GET_UPTIME = 0x00,
         GET_BATT_LVL = 0x01,

         /*************************************/
         /* Sensor-Actuator Microcontroller   */
         /*************************************/
         /* Differential Drive System Packets */
         SET_DDS_ENABLE = 0x10,
         SET_DDS_SPEED = 0x11,
         GET_DDS_SPEED  = 0x13,
         SET_DDS_PARAMS = 0x14,
         GET_DDS_PARAMS = 0x15,
         /* Accelerometer System Packets */
         GET_ACCEL_READING = 0x20,

         /*************************************/
         /* Power Management Microcontroller  */
         /*************************************/
         /* Switches */
         SET_SYSTEM_POWER_ENABLE = 0x39,
         SET_ACTUATOR_POWER_ENABLE = 0x40,
         SET_ACTUATOR_INPUT_LIMIT_OVERRIDE = 0x41,
         SET_USBIF_ENABLE = 0x42,
         /* Other */
         REQ_SOFT_PWDN = 0x43,
         GET_PM_STATUS = 0x44,
         GET_USB_STATUS = 0x45,

         /*************************************/
         /* Manipulator Microcontroller       */
         /*************************************/
         GET_CHARGER_STATUS = 0x60,
         /* Lift Actuator System */
         SET_LIFT_ACTUATOR_POSITION = 0x70,
         GET_LIFT_ACTUATOR_POSITION = 0x71,
         SET_LIFT_ACTUATOR_SPEED = 0x72,
         GET_LIMIT_SWITCH_STATE = 0x73,
         CALIBRATE_LIFT_ACTUATOR = 0x74,
         EMER_STOP_LIFT_ACTUATOR = 0x75,
         GET_LIFT_ACTUATOR_STATE = 0x76,
         /* Electromagnet Subsystem */
         SET_EM_CHARGE_ENABLE = 0x80,
         SET_EM_DISCHARGE_MODE = 0x81,
         GET_EM_ACCUM_VOLTAGE = 0x82,
         /* Range Finders */
         GET_RF_RANGE = 0x90,
         GET_RF_AMBIENT = 0x91,
         /* NFC Control */
         READ_NFC = 0xA0,
         WRITE_NFC = 0xA1,
         /* Remote I2C control */
         READ_SMBUS_BYTE = 0xC0,
         READ_SMBUS_BYTE_DATA = 0xC1,
         READ_SMBUS_WORD_DATA = 0xC2,
         READ_SMBUS_BLOCK_DATA = 0xC3,
         READ_SMBUS_I2C_BLOCK_DATA = 0xC4,
         WRITE_SMBUS_BYTE = 0xD0,
         WRITE_SMBUS_BYTE_DATA = 0xD1,
         WRITE_SMBUS_WORD_DATA = 0xD2,
         WRITE_SMBUS_BLOCK_DATA = 0xD3,
         WRITE_SMBUS_I2C_BLOCK_DATA = 0xD4,
         /*************************************/
         /* Invalid value for conversions     */
         /*************************************/
         INVALID = 0xFF
      };
      
   public:

      CPacket(uint8_t un_type_id,
              uint8_t un_data_length,
              uint8_t* pun_data) :
         m_unTypeId(un_type_id),
         m_unDataLength(un_data_length),
         m_punData(pun_data) {}
      
      EType GetType() const;
      
      bool HasData() const;
      uint8_t GetDataLength() const;
      const uint8_t* GetDataPointer() const;

   private: 
      uint8_t m_unTypeId;
      uint8_t m_unDataLength;
      uint8_t* m_punData;
   };

public:
   CPacketControlInterface(CHUARTController& c_controller) :
      m_eState(EState::SRCH_PREAMBLE1),
      m_unRxBufferPointer(0),
      m_unUsedBufferLength(0),
      m_cPacket(0xFF, 0, 0),
      m_cController(c_controller) {}

   EState GetState() const;

   const char* StateToString(EState e_state) const;

   const CPacket& GetPacket() const;
  
   void ProcessInput();

   void Reset();

   void SendPacket(CPacket::EType e_type,
                   const uint8_t* pun_tx_data,
                   uint8_t un_tx_data_length);
                   
   void SendPacket(CPacket::EType e_type,
                   uint8_t un_tx_data) {
      SendPacket(e_type, &un_tx_data, 1);                
   }
   
   void SendPacket(CPacket::EType e_type) {
      SendPacket(e_type, nullptr, 0);                
   }

private:
   uint8_t ComputeChecksum(uint8_t* pun_buf_data, uint8_t un_buf_length);
   void AdjustBuffer();
   void ReceiveFrame(uint8_t *pun_data, uint8_t un_length);

   EState m_eState;

   uint8_t m_unRxBufferPointer;
   uint8_t m_unUsedBufferLength;
   uint8_t m_punRxBuffer[RX_COMMAND_BUFFER_LENGTH];
   
   CPacket m_cPacket;

   CHUARTController& m_cController;
};
   
#endif
   
