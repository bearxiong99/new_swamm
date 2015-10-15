#ifndef __CLI_MESSAGE_H__
#define __CLI_MESSAGE_H__

#define MSG_DEFAULT			        "Are you sure? (y/n): "

#define MSG_NO_ENTRY                "No entry"

#define MSG_CMD_RESET_CODI			"Are you sure you want to reset CODI? (y/n): "
#define MSG_SUCC_RESET_CODI			"The CODI has been reset successfully"
#define MSG_FAIL_RESET_CODI			"Reset CODI failed"
#define MSG_NOT_PERMITTED_OPERATION "This operation is not permitted"

#define MSG_CMD_RESET_MOBILEDEV 	"Are you sure you want to reset mobile device? (y/n): "
#define MSG_SUCC_RESET_MOBILEDEV	"The mobile deveice has been reset successfully"
#define MSG_FAIL_RESET_MOBILEDEV	"Reset mobile device failed"

#define MSG_ADD_SENSOR				"New Sensor is added"
#define MSG_DELETE_SENSOR			"Are you sure you want to delete this sensor? (y/n): "
#define MSG_DELETE_SENSOR_ALL		"Are you sure you want to delete all sensors? (y/n): "
#define MSG_SUCC_DELETE_SENSORL		"The sensor is deleted from this MCU"
#define MSG_SUCC_DELETE_SENSOR_ALL	"All the sensors are deleted from this MCU"

#define MSG_DELETE_MEMBER			"Are you sure you want to delete this member? (y/n): "
#define MSG_DELETE_MEMBER_ALL		"Are you sure you want to delete all members? (y/n): "
#define MSG_SUCC_DELETE_MEMBER		"The member is deleted from this MCU"
#define MSG_SUCC_DELETE_MEMBER_ALL	"All the members are deleted from this MCU"

#define MSG_SUCC_ADD_REGISTER		"New coordinator register key is added"
#define MSG_SUCC_DELETE_REGISTER	"Coordinator register key is deleted"

#define MSG_READ_METER_ALL			"Do you want to read value for all meters? (y/n): "
#define MSG_READ_METER				"Do you want to read value for this meter? (y/n): "
#define MSG_SUCC_READ_METER_ALL		"The reading operation is performed for all meters"
#define MSG_SUCC_READ_METER			"The reading operation is performed for this meter(%s):"

#define MSG_WRITE_METER				"Do you want to write value for this meter? (y/n): "

#define MSG_READ_SENSOR				"Do you want to read value for this sensor? (y/n): "

#define MSG_INSTALL_MCU				"Do you want to install this MCU? (y/n): "
#define MSG_REQ_INSTALL_MCU			"This MCU is requested for installation to the FEP"
#define MSG_UNINSTALL_MCU			"Do you want to un-install this MCU? (y/n): "
#define MSG_REQ_UNINSTALL_MCU		"This MCU is requested for de-installation for the FEP"

#define MSG_TIMEZONE_WARNING		"WARNING: if you reset the Timezone, please pay special attention to that its operation\r\n is suspended and after a short time, it will be reboot.\r\nAre you sure? (y/n): "
#define MSG_RESET_WARNING			"WARNING: if you reset the MCU, please pay special attention to that its operation\r\n is suspended and after a short time, it will be reboot."
#define MSG_CMD_RESET_MCU			"Do you really want to reset this MCU? (y/n): "
#define MSG_RUN_RESET_MCU			"This MCU is now rebooting. please wait for a few minutes"

#define MSG_SHUTDOWN_WARNING		"WARNING: if you shutdown the MCU, please pay special attention to that its operation\r\n is suspended and after a short time, it will be power off. So after doing this, there is no way to access this MCU remotely."
#define MSG_CMD_SHUTDOWN_MCU		"Do you really want to shutdown this MCU? (y/n): "
#define MSG_RUN_SHUTDOWN_MCU		"This MCU is now shutdowning. please wait for a few minutes"


#define MSG_FACTORY_DEFAULT			"WARNING: Restoring factory presets will be brought about changing configuration\r\n of the MCU into initial one and deleting information from the MCU (connection parameters for FEP, metering data, all sensors information installed and tother configuration changed during its working)."
#define MSG_CMD_FACTORY_DEFAULT		"Do you really want to restore factory presets? (y/n): "
#define MSG_RUN_FACTORY_DEFAULT		"Now factory presets is being restored to the MCU. please wait a few minutes"

#define MSG_CMD_FIRMWARE_DIST		"Are you sure you want to download file? (y/n): "
#define MSG_CMD_FIRMWARE_UPDATE		"Are you sure you want to update MCU's Firmware? (y/n): "
#define MSG_INFO_FIRMWARE_UPDATE	"If ZMODEM program can NOT be executed automatically-in other words, you cant see\r\n 'Select Files to Send using ZModem' window, Run ZModem program manually.\r\n (To cancel uploading, press CTRL+X 2-3 times continuously"
#define MSG_RUN_FIRMWARE_UPDATE		"Now new firmware is being applied"

#define MSG_RUN_MONITOR_DISABLE		"The function of monitoring MCU is inactive now"
#define MSG_RUN_MONITOR_ENABLE		"The function of monitoring MCU is active now"

#define MSG_CMD_EMERG_RESET_MCU		"Do you really want to reset this MCU? (y/n): "
#define MSG_RUN_EMERG_RESET_MCU		"This MCU is now rebooting. please wait for a few minutes"

#define MSG_INFO_INVALID_CMD		"Invalid command"
#define MSG_INFO_OUT_OF_RANGE		"Out of range error"

#define MSG_SET_MCU_ID				"Please note that changing the MCU's ID will affect its working unexpectedly.\r\n The ID entered must be unique within whole system."
#define MSG_CMD_SET_MCU_ID			"Do you really want to set/change ID of this MCU? (y/n): "
#define MSG_INFO_SET_MCU_ID			"The ID of this MCU is changed to %0d successfully"
#define MSG_INFO_SET_MCU_NAME		"The name of this MCU is changed to '%s' successfully"
#define MSG_INFO_SET_MCU_DESCR		"The description of this MCU is changed to '%s' successfully"
#define MSG_INFO_SET_MCU_LOCATION	"The location information of this MCU is changed to '%s' successfully"
#define MSG_INFO_SET_MCU_CONTACT	"The contact information of this MCU is changed to '%s' successfully"
#define MSG_INFO_SET_MCU_TIME		"The current time of this MCU is changed to %s successfully"
#define MSG_INFO_SET_METER_ID		"The ID of the meter connected with this MCU is changed to '%s' successfully"
#define MSG_INFO_SET_TEMP_RANGE		"The range of the temperature is changed to '%.1f ~ %.1f' successfully"

#define MSG_INFO_SET_ACCESS_TYPE	"The access type of the MCU is changed to '%s' successfully"
#define MSG_INFO_SET_ACCESS_MODE	"The access protocol of the MCU is changed to '%s' successfully"
#define MSG_INFO_SET_MOBILE_APN		"The APN information of the MCU is changed to '%s' successfully"
#define MSG_INFO_SET_MOBILE_NO		"The mobile phone number of the MCU is changed to '%s' successfully"


#define MSG_INFO_SET_FEP_IP			"The IP address of FEP is changed to '%s' successfully"
#define MSG_INFO_SET_FEP_PORT		"The port number of FEP is changed to %0d successfully"
#define MSG_INFO_SET_FEP_ALARM_PORT	"The port number for alarm of FEP is changed to %0d successfully"
#define MSG_INFO_SET_FEP_SECURITY_STATE "The Security Mode of FEP is changed to '%s' successfully"

#define MSG_INFO_SET_MCU_LISTEN_PORT	"The listening port number of the MCU is changed to %0d successfully.\xd\xa\xd\xaTo apply new listen port, MCU must be reboot"
#define MSG_CMD_MCU_REBOOT		"Do you really want to reboot this MCU? (y/n): "
#define MSG_INFO_SET_MCU_LOCAL_IP	"The IP address of the MCU is changed successfully."

#define MSG_CMD_SET_ETHER_TYPE		"Do you want to set type of ethernet? (y/n): "
#define MSG_INFO_SET_ETHER_TYPE		"The ethernet type information of the MCU is changed to '%s(%0d)' successfully."

#define MSG_CMD_SET_CODI_CHANNEL	"Do you really want to change channel ID of CODI? (y/n): "
#define MSG_INFO_SET_CODI_CHANNEL	"The channel ID of CODI is changed to %0d successfully."

#define MSG_CMD_SET_CODI_PANID		"Do you really want to change PAN ID of CODI? (y/n): "
#define MSG_INFO_SET_CODI_PANID		"The PAN ID of CODI is changed to %0d successfully."

#define MSG_CMD_SET_CODI_RFPOWER	"Do you really want to change RF Power of CODI? (y/n): "
#define MSG_INFO_SET_CODI_RFPOWER	"The RF Power of CODI is changed to %0d successfully."

#define MSG_CMD_SET_SENSOR_PANID			"Do you really want to change panid? (y/n): "
#define MSG_INFO_SET_SENSOR_PANID		"The PAN ID of SENSOR is changed to %0d successfully."

#define MSG_CMD_SET_SENSOR_CHANNEL			"Do you really want to change channel? (y/n): "
#define MSG_INFO_SET_SENSOR_CHANNEL	"The channel of SENSOR is changed to %0d successfully."

#define MSG_CMD_SET_SENSOR_ROUTING			"Do you really want to change routing? (y/n): "

#define MSG_CMD_SET_SENSOR_RFPOWER	"Do you really want to change RF Power of SENSOR? (y/n): "
#define MSG_INFO_SET_SENSOR_RFPOWER	"The RF Power of SENSOR is changed to %0d successfully."

#define MSG_USE_MISSING_READ		"Do you want to enable 'Missing meter data reading' function? (y/n): "
#define MSG_INFO_USE_MISSING_READ	"The 'Missing meter data reading' function is enabled."

#define MSG_USE_OPERATION_LOG		"Do you want to enable 'Operation logging' function? (y/n): "
#define MSG_INFO_USE_OPERATION_LOG	"The 'Operation logging' function is enabled."

#define MSG_USE_COMM_LOG			"Do you want to enable 'Communication logging' function? (y/n): "
#define MSG_INFO_USE_COMM_LOG		"The 'Communication logging' function is enabled."

#define MSG_USE_AUTO_SENSOR_REG		"Do you want to enable 'Automatic sensor registration' function? (y/n): "
#define MSG_INFO_USE_AUTO_SENSOR_REG	"The 'Automatic sensor registration' function is enabled."

#define MSG_USE_AUTO_RESET			"Are you sure you want to enable 'Automatic reset' function? (y/n): "
#define MSG_INFO_USE_AUTO_RESET		"The 'Automatic reset' function is enabled."

#define MSG_USE_AUTO_TIME_SYNC		"Are you sure you want to enable 'Automatic time synchronization' function? (y/n): "
#define MSG_INFO_USE_AUTO_TIME_SYNC	"The 'Automatic time synchronization' function is enabled."

#define MSG_USE_AUTO_CODI_RESET		"Are you sure you want to enable 'Automatic CODI reset' function? (y/n): "
#define MSG_INFO_USE_AUTO_CODI_RESET	"The 'Automatic CODI reset' function is enabled."

#define MSG_USE_SECONDARY_NETWORK	"Are you sure you want to use secondary network? (y/n): "
#define MSG_INFO_USE_SECONDARY_NETWORK	"The secondary network is activated."

#define MSG_USE_MOBILE_COMM_LOG		"Do you want to enable 'Mobile communication logging' function? (y/n): "
#define MSG_INFO_USE_MOBILE_COMM_LOG	"The 'Mobile communication logging' function is enabled."

#define MSG_USE_AUTO_SEND_DATA		"Are you sure you want to enable 'Automatic data transfer to FEP' function? (y/n): "
#define MSG_INFO_USE_AUTO_SEND_DATA	"The 'Automatic data transfer to FEP' function is enabled."

#define MSG_USE_COMM_SECURITY		"Are you sure you want to use 'communication security mechanizm' function? (y/n): "
#define MSG_INFO_USE_COMM_SECURITY	"The 'communication security mechanizm' function is enabled."

#define MSG_DISABLE_MISSING_READ	"Do you want to disable 'Missing meter data reading' function? (y/n): "
#define MSG_INFO_DISABLE_MISSING_READ	"The 'Missing meter data reading' function is disabled."

#define MSG_DISABLE_OPERATION_LOG	"Do you want to disable 'Operation logging' function? (y/n): "
#define MSG_INFO_DISABLE_OPERATION_LOG	"The 'Operation logging' function is disabled."

#define MSG_DISABLE_COMM_LOG		"Do you want to disable 'Communication logging' function? (y/n): "
#define MSG_INFO_DISABLE_COMM_LOG	"The 'Communication logging' function is disabled."

#define MSG_DISABLE_AUTO_SENSOR_REG	"Do you want to disable 'Automatic sensor registration' function? (y/n): "
#define MSG_INFO_DISABLE_AUTO_SENSOR_REG	"The 'Automatic sensor registration' function is disabled."

#define MSG_DISABLE_AUTO_RESET		"Are you sure you want to disable 'Automatic reset' function? (y/n): "
#define MSG_INFO_DISABLE_AUTO_RESET	"The 'Automatic reset' function is disabled."

#define MSG_DISABLE_AUTO_TIME_SYNC	"Are you sure you want to disable 'Automatic time synchronization' function? (y/n): "
#define MSG_INFO_DISABLE_AUTO_TIME_SYNC	"The 'Automatic time synchronization' function is disabled."

#define MSG_DISABLE_AUTO_CODI_RESET	"Are you sure you want to disable 'Automatic CODI reset' function? (y/n): "
#define MSG_INFO_DISABLE_AUTO_CODI_RESET	"The 'Automatic CODI reset' function is disabled."

#define MSG_DISABLE_SECONDARY_NETWORK	"Are you sure you want to NOT use secondary network? (y/n): "
#define MSG_INFO_DISABLE_SECONDARY_NETWORK	"The secondary network is de-activated."

#define MSG_DISABLE_MOBILE_COMM_LOG	"Do you want to disable 'Mobile communication logging' function? (y/n): "
#define MSG_INFO_DISABLE_MOBILE_COMM_LOG	"The 'Mobile communication logging' function is disabled."

#define MSG_DISABLE_AUTO_SEND_DATA	"Are you sure you want to disable 'Automatic data transfer to FEP' function? (y/n): "
#define MSG_INFO_DISABLE_AUTO_SEND_DATA	"The 'Automatic data transfer to FEP' function is disabled."

#define MSG_DISABLE_COMM_SECURITY	"Are you sure you want to NOT use 'communication security mechanizm' function? (y/n): "
#define MSG_INFO_DISABLE_COMM_SECURITY	"The 'communication security mechanizm' function is disabled."

#define MSG_CMD_SET_PPP_ACCOUNT		"Do you want to change PPP account information? (y/n): "
#define MSG_INFO_SET_PPP_ACCOUNT	"The account information of the PPP is changed to '%s' successfully"

#define MSG_CMD_SET_PPP_PWD		"Do you want to change PPP password ? (y/n): "
#define MSG_INFO_SET_PPP_PWD		"The password of the PPP is changed successfully"

#define MSG_CMD_SET_GPIO_PORT		"Do you really want to change GPIO Port setting? (y/n): "
#define MSG_INFO_SET_GPIO_PORT		"The value of the GPIO port %d is changed to %d successfully"


#define MSG_FUNC_USABILITY		"(1) Function Summary"
#define MSG_FUNC_MISSING_READ		"Missing meter data reading"
#define MSG_FUNC_OPERATION_LOG		"Operation logging"
#define MSG_FUNC_AUTO_SENSOR_REG	"Automatic sensor registration"
#define MSG_FUNC_AUTO_MCU_RESET		"Automatic MCU reset"
#define MSG_FUNC_AUTO_TIME_SYNC		"Automatic MCU time synchronization"
#define MSG_FUNC_AUTO_CODI_RESET	"Automatic CODI reset"
#define MSG_FUNC_USE_SECOND_NET		"Use secondary network"
#define MSG_FUNC_MOBILE_COMM_STAT	"Mobile communication statistics logging"
#define MSG_FUNC_AUTO_SEND_DATA		"Automatic data transfer to FEP"
#define MSG_FUNC_COMM_SECURITY		"Communication security mechanizm"

#define MSG_PARAM_OF_FUNC		"(2) Parameter Information"
#define MSG_PARAM_STARTUP_TIME		"Start-up Time"
#define MSG_PARAM_MTR_READING_DAY_MASK	"Meter Reading Day Mask"
#define MSG_PARAM_MISSING_DATA_READING_DAY_MASK	"Missing Meter Data Reading Day Mask"
#define MSG_PARAM_MTR_READING_HOUR_MASK	"Meter Reading Hour Mask"
#define MSG_PARAM_MISSING_DATA_READING_HOUR_MASK	"Missing Meter Data Reading Hour Mask"
#define MSG_PARAM_MTR_READING_MIN_MASK	"Meter Reading Min Mask"
#define MSG_PARAM_MISSING_DATA_READING_MIN_MASK	"Missing Meter Data Reading Min Mask"

#define MSG_PARAM_NUM_METER_ONE_READING	"Number of meters in one reading operation"
#define MSG_PARAM_SAVE_MTR_DATA_COUNT	"Metering data count to be saved"
#define MSG_PARAM_DATA_UPLOAD_TYPE	"Type of schedule for uploading data"
#define MSG_PARAM_DATA_UPLOAD_SCHEDULE	"Schedule for uploading data"
#define MSG_PARAM_DATA_UPLOAD_HOUR	"Data upload hour"
#define MSG_PARAM_DATA_UPLOAD_RETRIES	"Retry count for data uploading"
#define MSG_PARAM_OPERATION_LOG_DAYS	"Operation logging days"
#define MSG_PARAM_EVENT_LOG_DAYS	"Event logging days"
#define MSG_PARAM_COMM_LOG_DAYS		"Communication logging days"
#define MSG_PARAM_METERING_DATA_LOG_DAYS	"Metering data logging days"

#define MSG_PARAM_AUTO_MCU_RESET_TIME	"Automatic MCU reset time"
#define MSG_PARAM_AUTO_MCU_RESET_INTERVAL	"Automatic MCU reset interval"
#define MSG_PARAM_POWER_OFF_DELAY	"Power off delay"
#define MSG_PARAM_TEMP_READING_INTERVAL	"Temperature reading interval"
#define MSG_PARAM_AUTO_TIME_SYNC_INTERVAL	"Automatic time synchronization interval"
#define MSG_PARAM_CODI_POLL_INTERVAL	"CODI polling interval"
#define MSG_PARAM_CODI_RESET_INTERVAL	"CODI reset interval"
#define MSG_PARAM_CODI_RESET_DELAY	"CODI reset delay"
#define MSG_PARAM_CODI_LEDOFF_TIME	"CODI LED off time"
#define MSG_PARAM_CODI_ACK_WAIT_TIME	"CODI ACK wait time"
#define MSG_PARAM_SENSOR_TIMEOUT	"Sensor timeout"
#define MSG_PARAM_MAX_SENSOR_ALIVE_TIME	"Maximum Sensor communication delay"
#define MSG_PARAM_THRESHOLD		"Thresold setting"
#define MSG_PARAM_SECOND_NETWORK	"Secondary network"


#define MSG_CLI_PARAM_AID		"All or specific sensor ID (minimum 4 characters)"
#define MSG_CLI_PARAM_SID		"CODI ID(minimum 4 characters)"
#define MSG_CLI_PARAM_ASID		"All or specific CODI ID(minimum 4 characters)"
#define MSG_CLI_PARAM_IF		"Interface name"
#define MSG_CLI_PARAM_AIF		"All or specific interface name"
#define MSG_CLI_PARAM_PT		"Port name"
#define MSG_CLI_PARAM_STYPE		"Type"
#define MSG_CLI_PARAM_FID		"Sensor ID (16 characters)"
#define MSG_CLI_PARAM_MTT		"Type of meter"
#define MSG_CLI_PARAM_FEPIP		"IP address of FEP server"
#define MSG_CLI_PARAM_FEPPORT		"Port number of FEP server"
#define MSG_CLI_PARAM_IP		"IP Address"
#define MSG_CLI_PARAM_NETMASK		"Subnet mask"
#define MSG_CLI_PARAM_GW		"IP Address of gateway"
#define MSG_CLI_PARAM_PORT		"Port number"
#define MSG_CLI_PARAM_MCUID		"MCU ID (16 characters)"
#define MSG_CLI_PARAM_ETHERNET		"Ethernet type"
#define MSG_CLI_PARAM_MOBILE		"Module type for mobile"
#define MSG_CLI_PARAM_MC		"Level of meter"
#define MSG_CLI_PARAM_MTR_VENDOR	"Vendor of meter"
#define MSG_CLI_PARAM_NUM		"Value"
#define MSG_CLI_PARAM_MT		"Type of meter(1-6)"
#define MSG_CLI_PARAM_MD		"Metering day"
#define MSG_CLI_PARAM_MP		"Metering data point"
#define MSG_CLI_PARAM_PI		"Initial value"
#define MSG_CLI_PARAM_SN		"Serial number of meter"
#define MSG_CLI_PARAM_TP		"Topology of meter"
#define MSG_CLI_PARAM_MHH		"Meter reading hour"
#define MSG_CLI_PARAM_YY		"Year"
#define MSG_CLI_PARAM_MM		"Month"
#define MSG_CLI_PARAM_DD		"Day"
#define MSG_CLI_PARAM_HH		"Hour"
#define MSG_CLI_PARAM_NN		"Min"
#define MSG_CLI_PARAM_SS		"Sec"
#define MSG_CLI_PARAM_STR		"String"
#define MSG_CLI_PARAM_PUSER		"User ID"
#define MSG_CLI_PARAM_PPASS		"Password"
#define MSG_CLI_PARAM_PS16		"Password"
#define MSG_CLI_PARAM_PGROUP		"Group(admin,user,guest)"
#define MSG_CLI_PARAM_HNAME		"Name of host"
#define MSG_CLI_PARAM_HADDR		"IP address of host"
#define MSG_CLI_PARAM_HPFROM		"Port number (start)"
#define MSG_CLI_PARAM_HPTO		"Port number (end)"
#define MSG_CLI_PARAM_DAY		"Day"
#define MSG_CLI_PARAM_TMIN		"Temperature (minimum)"
#define MSG_CLI_PARAM_TMAX		"Temperature (maximum)"
#define MSG_CLI_PARAM_GPPORT		"GPIO Port number(decimal format)"
#define MSG_CLI_PARAM_GPNUM		"GPIO Value (0/1)"
#define MSG_CLI_PARAM_CH		"Channel Number"
#define MSG_CLI_PARAM_BIND		"Bind Number"
#define MSG_CLI_PARAM_PWR		"RF signal strength"
#define MSG_CLI_PARAM_ROUTING		"disable=inactive, enable=active"
#define MSG_CLI_PARAM_COMMTYPE		"dgrm=Datagram, seq=Sequence"
#define MSG_CLI_PARAM_SPEED		"Baudrate (2400,4800,9600,...)"
#define MSG_CLI_PARAM_TIMEOUT		"Timeout"
#define MSG_CLI_PARAM_KEEPALIVE		"Keep Alive Time"
#define MSG_CLI_PARAM_ETYPE		"Type of event"
#define MSG_CLI_PARAM_PAGE		"Number of page"
#define MSG_CLI_PARAM_ENABLE		"Setting (enable, disable)"
#define MSG_CLI_PARAM_LBT		"Delay (10~255 ms)"
#define MSG_CLI_PARAM_ADATE		"Year,Month, Day (all, yyyymmdd)"

#define MSG_SLEEPY_FFD_NOT_SUPPORT      "Sleepy FFD does not support"

#endif	// __CLI_MESSAGE_H__
