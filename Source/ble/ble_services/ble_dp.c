/* Copyright (c) 2013 Vasco Data Security. All Rights Reserved.
 *
 * The information contained herein is property of Vasco Data Security.
 *
 */

#include "ble_dp.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_l2cap.h"
//#include "ble_sdk_util.h"
#include "app_util.h"

#define INVALID_OTP		255

/**@brief Connect event handler.
 *
 * @param[in]   p_dp       Digipass Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
static void on_connect(ble_dp_t * p_dp, ble_evt_t * p_ble_evt)
{
    p_dp->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Disconnect event handler.
 *
 * @param[in]   p_dp       Digipass Service structure.
 * @param[in]   p_dp_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_dp_t * p_dp, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_dp->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**@brief Write event handler.
 *
 * @param[in]   p_bas       Digipass Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_dp_t * p_bas, ble_evt_t * p_ble_evt)
{

}

void ble_dp_on_ble_evt(ble_dp_t * p_dp, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id) //See ble_gap.h line 409
    {
				/**< Connection established. */ 
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_dp, p_ble_evt);
            break;
				
				/**< Disconnected from peer. */
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_dp, p_ble_evt);
            break;
            
				/**< Write operation performed. */
        case BLE_GATTS_EVT_WRITE:
            on_write(p_dp, p_ble_evt);
            break;
            
        default:
            break;
    }
}

/**@brief Digipass OTP characteristic.
 *
 * @param[in]   p_dp        Digipass Service structure.
 * @param[in]   p_dp_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t dp_otp_char_add(ble_dp_t * p_dp, const ble_dp_init_t * p_dp_init)
{
    ble_gatts_char_md_t char_md;
    //ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    uint8_t             initial_otp;

    memset(&char_md, 0, sizeof(char_md));
    
		char_md.char_props.read   = 1; //Reading value permitted.
    //char_md.char_properties.notify = (p_bas->is_notification_supported) ? 1 : 0;
    char_md.p_char_user_desc       = NULL; //Pointer to a UTF-8, NULL if the descriptor is not required.
    char_md.p_char_pf              = NULL; //Pointer to a presentation format structure or NULL if the descriptor is not required.
    char_md.p_user_desc_md         = NULL; //Attribute Metadata for the User Description descriptor, or NULL for default values.
    //char_md.p_cccd_md              = (p_bas->is_notification_supported) ? &cccd_md : NULL;
		char_md.p_cccd_md							 = NULL;
		//Attribute Metadata for the Client Characteristic Configuration Descriptor, or NULL for default values.
    char_md.p_sccd_md              = NULL; 
		//Attribute Metadata for the Server Characteristic Configuration Descriptor, or NULL for default values.
    
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_DP_OTP); //Assing Digipass OTP UUID
    
    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_dp_init->dp_otp_char_attr_md.read_perm; //Read permissions.
    attr_md.write_perm = p_dp_init->dp_otp_char_attr_md.write_perm; //Write permissions.
    attr_md.vloc       = BLE_GATTS_VLOC_STACK; /**< Value is located in stack memory. */
    attr_md.rd_auth    = 0;  //Value will be requested from application on every read. OFF
    attr_md.wr_auth    = 0; //Value will be reported to application on every write. OFF
    attr_md.vlen       = 0; //Variable length attribute. OFF
    
    initial_otp = p_dp_init->initial_otp;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid; //Pointer to the attribute UUID.
    attr_char_value.p_attr_md    = &attr_md; //Pointer to the attribute metadata structure.
    attr_char_value.init_len     = sizeof(uint8_t); //Initial attribute value length in bytes.
    attr_char_value.init_offs    = 0; 
		//Initial attribute value offset in bytes. If different from zero, the first init_offs bytes of the attribute value will be left uninitialized.
    attr_char_value.max_len      = sizeof(uint8_t); //Maximum attribute value length in bytes.
    attr_char_value.p_value      = &initial_otp; //Pointer to the attribute data.
    
    return sd_ble_gatts_characteristic_add(p_dp->service_handle, 
																						&char_md,
                                            &attr_char_value,
                                            &p_dp->dp_otp_handles);
}

uint32_t ble_otp_init(ble_dp_t * p_dp, const ble_dp_init_t * p_dp_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_dp->evt_handler               = p_dp_init->evt_handler;
    p_dp->conn_handle               = BLE_CONN_HANDLE_INVALID;
    p_dp->otp_last        					= INVALID_OTP;
    
    // Add service
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_DP_OTP);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_dp->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    // Add OTP characteristic
    return dp_otp_char_add(p_dp, p_dp_init);
}


/** NOT used at this moment, OTP update routine **/
uint32_t ble_otp_update(ble_dp_t * p_dp, uint8_t dp_otp)
{
    uint32_t err_code = NRF_SUCCESS;

    if (dp_otp != p_dp->otp_last)
    {
        uint16_t len = sizeof(uint8_t);
        
        // Save new OTP value
        p_dp->otp_last = dp_otp;
        
        // Update database
        err_code = sd_ble_gatts_value_set(p_dp->dp_otp_handles.value_handle,
                                          0,
                                          &len,
                                          &dp_otp);
		}

    return err_code;
}
