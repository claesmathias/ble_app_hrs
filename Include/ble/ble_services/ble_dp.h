/* Copyright (c) 2013 Vasco Data Security. All Rights Reserved.
 *
 * The information contained herein is property of Vasco Data Security.
 *
 */

/** @file
 *
 * @defgroup ble_sdk_dp Digipass Service
 * @brief Digipass Service module.
 *
 * @details This module implements the Digipass Service.
 */

#ifndef BLE_DP_H__
#define BLE_DP_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

/** @defgroup UUID_CHARACTERISTICS Characteristic UUID definitions
 * @{ */
#define BLE_UUID_DP_OTP	                              0x1FFF     /**< Digipass OTP characteristic UUID. */
/** @} */


/**@brief Digipass Service event type. */
typedef enum
{
    BLE_DP_EVT_NOTIFICATION_ENABLED,                   /**< Digipass value notification enabled event. */
    BLE_DP_EVT_NOTIFICATION_DISABLED                   /**< Digipass value notification disabled event. */
} ble_dp_evt_type_t;

/**@brief Digipass Service event. */
typedef struct
{
    ble_dp_evt_type_t evt_type;                        /**< Type of event. */
} ble_dp_evt_t;

// Forward declaration of the ble_dp_t type. 
typedef struct ble_dp_s ble_dp_t;

/**@brief Digipass Service event handler type. */
typedef void (*ble_dp_evt_handler_t) (ble_dp_t * p_bas, ble_dp_evt_t * p_evt);

/**@brief Digipass Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_dp_evt_handler_t          evt_handler;                    /**< Event handler to be called for handling events in the Digipass Service. */
    ble_srv_report_ref_t *        p_report_ref;                   /**< If not NULL, a Report Reference descriptor with the specified value will be added to the Digipass OTP characteristic */
    uint8_t                       initial_otp;             				/**< Initial OTP */
    ble_srv_cccd_security_mode_t  dp_otp_char_attr_md;     						/**< Initial security level for digipass characteristics attribute */
    ble_gap_conn_sec_mode_t       dp_level_report_read_perm; 			/**< Initial security level for digipass report read attribute */
} ble_dp_init_t;

/**@brief Digipass structure. This contains various status information for the service. */
typedef struct ble_dp_s
{
    ble_dp_evt_handler_t          evt_handler;                    /**< Event handler to be called for handling events in the Digipass Service. */
    uint16_t                      service_handle;                 /**< Handle of Digipass Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t      dp_otp_handles;          				/**< Handles related to the Digipass OTP characteristic. */
		uint8_t                       otp_last;             					/**< Last OTP passed to the Digipass Service. */
    uint16_t                      conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                          is_notification_supported;      /**< TRUE if notification of Battery Level is supported. */
} ble_dp_t;

/**@brief Initialize the Digipass Service.
 *
 * @param[out]  p_dp       Digipass Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_dp_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_otp_init(ble_dp_t * p_dp, const ble_dp_init_t * p_dp_init);

/**@brief Digipass Service BLE stack event handler.
 *
 * @details Handles all events from the BLE stack of interest to the Digipass Service.
 *
 * @note 
 *
 * @param[in]   p_dp       Digipass Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_dp_on_ble_evt(ble_dp_t * p_dp, ble_evt_t * p_dp_evt);

/**@brief Update otp.
 *
 * @details The application calls this function after having pushed on the otp button. If
 *          notification has been enabled, the otp characteristic is sent to the client.
 *
 * @note 
 *
 * @param[in]   p_dp           Digipass Service structure.
 * @param[in]   otp  					 OTP.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_otp_update(ble_dp_t * p_dp, uint8_t otp);

#endif // BLE_DP_H__

/** @} */
