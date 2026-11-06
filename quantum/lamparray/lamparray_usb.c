// Copyright 2024 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include "lamparray.h"
#include "usb_types.h"
#include "usb_endpoints.h"
#include "usb_report_handling.h"

#ifdef LAMPARRAY_ENABLE

static universal_lamparray_response_t          lamparray_report_buf;
static lamparray_attributes_report_t           lamparray_attributes_buf;
static lamparray_attributes_response_report_t  lamparray_attributes_response_buf;

static void lamparray_transfer_cb(USBDriver *usbp) {
    if (lamparray_report_buf.report_id == LAMPARRAY_REPORT_ID_ATTRIBUTES_REQUEST) {
        lamparray_set_attributes_response(lamparray_report_buf.lamp_id);
    } else {
        lamparray_queue_request(&lamparray_report_buf);
    }
}

bool lamparray_usb_request_cb(USBDriver *usbp) {
    usb_control_request_t *setup = (usb_control_request_t *)usbp->setup;

    if ((setup->bmRequestType & (USB_RTYPE_TYPE_MASK | USB_RTYPE_RECIPIENT_MASK)) != (USB_RTYPE_TYPE_CLASS | USB_RTYPE_RECIPIENT_INTERFACE)) {
        return false;
    }

    if (setup->wIndex != LAMPARRAY_INTERFACE) {
        return false;
    }

    if ((setup->bmRequestType & USB_RTYPE_DIR_MASK) == USB_RTYPE_DIR_DEV2HOST) {
        if (setup->bRequest != HID_REQ_GetReport) {
            return false;
        }

        switch (setup->wValue.lbyte) {
            case LAMPARRAY_REPORT_ID_ATTRIBUTES:
                lamparray_attributes_buf.report_id = LAMPARRAY_REPORT_ID_ATTRIBUTES;
                lamparray_get_attributes(&lamparray_attributes_buf.attributes);
                usbSetupTransfer(usbp, (uint8_t *)&lamparray_attributes_buf, sizeof(lamparray_attributes_buf), NULL);
                return true;
            case LAMPARRAY_REPORT_ID_ATTRIBUTES_RESPONSE:
                lamparray_attributes_response_buf.report_id = LAMPARRAY_REPORT_ID_ATTRIBUTES_RESPONSE;
                lamparray_get_attributes_response(&lamparray_attributes_response_buf.attributes_response);
                usbSetupTransfer(usbp, (uint8_t *)&lamparray_attributes_response_buf, sizeof(lamparray_attributes_response_buf), NULL);
                return true;
        }

        return false;
    }

    if (setup->bRequest != HID_REQ_SetReport) {
        return false;
    }

    switch (setup->wValue.lbyte) {
        case LAMPARRAY_REPORT_ID_ATTRIBUTES_REQUEST:
        case LAMPARRAY_REPORT_ID_RANGE_UPDATE:
        case LAMPARRAY_REPORT_ID_MULTI_UPDATE:
        case LAMPARRAY_REPORT_ID_CONTROL:
            usbSetupTransfer(usbp, (uint8_t *)&lamparray_report_buf, sizeof(lamparray_report_buf), lamparray_transfer_cb);
            return true;
    }

    return false;
}

#endif
