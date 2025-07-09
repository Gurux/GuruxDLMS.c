//
// --------------------------------------------------------------------------
//  Gurux Ltd
//
//
//
// Filename:        $HeadURL$
//
// Version:         $Revision$,
//                  $Date$
//                  $Author$
//
// Copyright (c) Gurux Ltd
//
//---------------------------------------------------------------------------
//
//  DESCRIPTION
//
// This file is a part of Gurux Device Framework.
//
// Gurux Device Framework is Open Source software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; version 2 of the License.
// Gurux Device Framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// This code is licensed under the GNU General Public License v2.
// Full text may be retrieved at http://www.gnu.org/licenses/gpl-2.0.txt
//---------------------------------------------------------------------------

#include "../include/gxignore.h"
#ifndef DLMS_IGNORE_SERVER

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#endif

#include "../include/gxmem.h"
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include <string.h>

#include "../include/cosem.h"
#include "../include/dlms.h"
#include "../include/gxset.h"
#include "../include/gxinvoke.h"
#include "../include/helpers.h"
#include "../include/objectarray.h"
#include "../include/ciphering.h"
#include "../include/gxget.h"
#include "../include/gxkey.h"
#include "../include/serverevents.h"
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
#include "../include/gxecdsa.h"
#include "../include/gxpkcs10.h"
#include "../include/gx509Certificate.h"
#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)

#ifndef DLMS_IGNORE_CHARGE

int invoke_Charge(
    gxCharge* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    gxChargeTable* ct, * it;
    int ret = 0, pos;
    //Update unit charge.
    if (index == 1)
    {
        for (pos = 0; pos != object->unitChargePassive.chargeTables.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            ret = arr_getByIndex(&object->unitChargePassive.chargeTables, pos, (void**)&ct, sizeof(gxCharge));
#else
            ret = arr_getByIndex(&object->unitChargePassive.chargeTables, pos, (void**)&ct);
#endif //DLMS_IGNORE_MALLOC
            if (ret != 0)
            {
                return ret;
            }
            ct->chargePerUnit = (short)var_toInteger(value);
        }
    }
    //Activate passive unit charge.
    else if (index == 2)
    {
        object->unitChargeActive.chargePerUnitScaling = object->unitChargePassive.chargePerUnitScaling;
        object->unitChargeActive.commodity = object->unitChargePassive.commodity;
        for (pos = 0; pos != object->unitChargePassive.chargeTables.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->unitChargePassive.chargeTables, pos, (void**)&ct, sizeof(gxChargeTable))) != 0 ||
                (ret = arr_getByIndex(&object->unitChargeActive.chargeTables, object->unitChargeActive.chargeTables.size, (void**)&it, sizeof(gxChargeTable))) != 0)
            {
                break;
            }
            ++object->unitChargeActive.chargeTables.size;
#else
            if ((ret = arr_getByIndex(&object->unitChargePassive.chargeTables, pos, (void**)&ct)) != 0)
            {
                break;
            }
            it = (gxChargeTable*)gxmalloc(sizeof(gxChargeTable));
            arr_push(&object->unitChargeActive.chargeTables, it);
#endif //DLMS_IGNORE_MALLOC
            it->chargePerUnit = ct->chargePerUnit;
            it->index = ct->index;
        }
    }
    //Update total amount remaining.
    else if (index == 4)
    {
        object->totalAmountRemaining += var_toInteger(value);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_CREDIT
int invoke_Credit(
    gxCredit* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret = 0;
#ifdef DLMS_IGNORE_MALLOC
    uint32_t tmp;
#endif //DLMS_IGNORE_MALLOC

    //Update amount.
    if (index == 1)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_getUInt32(value->byteArr, &tmp)) == 0)
        {
            object->currentCreditAmount += tmp;
        }
#else
        object->currentCreditAmount += var_toInteger(value);
#endif //DLMS_IGNORE_MALLOC
    }
    //Set amount to value.
    else if (index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_getUInt32(value->byteArr, &tmp)) == 0)
        {
            object->currentCreditAmount = tmp;
        }
#else
        object->currentCreditAmount = var_toInteger(value);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 3)
    {
        // The mechanism for selecting the Credit is not in the scope of COSEM and
        // shall be specified by the implementer (e.g. button push, meter process, script etc.).
        object->status |= 4;
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
int invoke_gxTokenGateway(
    gxTokenGateway* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret = 0;
    //Update token.
    if (index == 1)
    {
        bb_clear(&object->token);
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctetString(value->byteArr, &object->token);
#else
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            bb_set(&object->token, value->byteArr->data, value->byteArr->size);
        }
        else
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_TOKEN_GATEWAY

#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
int invoke_AssociationLogicalName(
    dlmsServerSettings* settings,
    gxValueEventArg* e)
{
    int ret = 0;
    gxAssociationLogicalName* object = (gxAssociationLogicalName*)e->target;
    // Check reply_to_HLS_authentication
    if (e->index == 1)
    {
#ifndef DLMS_IGNORE_HIGH_GMAC
        unsigned char ch;
        gxByteBuffer tmp;
#endif //DLMS_IGNORE_HIGH_GMAC
        unsigned char tmp2[64];
        unsigned char equal;
        uint32_t ic = 0;
        gxByteBuffer bb;
        gxByteBuffer* readSecret;
        BYTE_BUFFER_INIT(&bb);
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count;
        if ((ret = bb_getUInt8(e->parameters.byteArr, &ch)) != 0)
        {
            return ret;
        }
        if (ch != DLMS_DATA_TYPE_OCTET_STRING)
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("High authentication failed. ", DLMS_ERROR_CODE_INVALID_PARAMETER);
#endif //DLMS_DEBUG
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        if ((ret = hlp_getObjectCount2(e->parameters.byteArr, &count)) != 0)
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("High authentication failed. ", DLMS_ERROR_CODE_INVALID_PARAMETER);
#endif //DLMS_DEBUG
            return ret;
        }
        if (count > bb_available(e->parameters.byteArr))
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("High authentication failed. ", DLMS_ERROR_CODE_INVALID_PARAMETER);
#endif //DLMS_DEBUG
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_HIGH_GMAC
        if (settings->base.authentication == DLMS_AUTHENTICATION_HIGH_GMAC)
        {
            bb_attach(&tmp, settings->base.sourceSystemTitle, sizeof(settings->base.sourceSystemTitle), sizeof(settings->base.sourceSystemTitle));
            if ((ret = bb_getUInt8(e->parameters.byteArr, &ch)) != 0 ||
                (ret = bb_getUInt32(e->parameters.byteArr, &ic)) != 0)
            {
                return ret;
            }
            //IC is also compared.
            e->parameters.byteArr->position -= 5;
            readSecret = &tmp;
        }
        else
#endif //DLMS_IGNORE_MALLOC
        {
            readSecret = &object->secret;
        }

        BYTE_BUFFER_INIT(&bb);
        bb_attach(&bb, tmp2, 0, sizeof(tmp2));
        if ((ret = dlms_secure(&settings->base,
            ic,
            &settings->base.stoCChallenge,
            readSecret,
            &bb)) != 0)
        {
            return ret;
        }
        equal = bb_available(e->parameters.byteArr) != 0 &&
            bb_compare(&bb,
                e->parameters.byteArr->data + e->parameters.byteArr->position,
                bb_available(e->parameters.byteArr));
        bb_clear(&settings->info.data);
        if (equal)
        {
#ifndef DLMS_IGNORE_HIGH_GMAC
            if (settings->base.authentication == DLMS_AUTHENTICATION_HIGH_GMAC)
            {
#ifdef DLMS_IGNORE_MALLOC
                bb_attach(&tmp, settings->base.cipher.systemTitle, sizeof(settings->base.cipher.systemTitle), sizeof(settings->base.cipher.systemTitle));
                readSecret = &tmp;
#else
                readSecret = &settings->base.cipher.systemTitle;
#endif //DLMS_IGNORE_MALLOC
                ic = settings->base.cipher.invocationCounter;
            }
#endif //DLMS_IGNORE_HIGH_GMAC
            e->byteArray = 1;
            if ((ret = dlms_secure(&settings->base,
                ic,
                &settings->base.ctoSChallenge,
                readSecret,
                &settings->info.data)) != 0)
            {
                return ret;
            }
            bb_insertUInt8(&settings->info.data, 0, DLMS_DATA_TYPE_OCTET_STRING);
            //bb_insertUInt8 is increasing the size by one.
            --settings->info.data.size;
            bb_insertUInt8(&settings->info.data, 1, (unsigned char)(settings->info.data.size));
            object->associationStatus = DLMS_ASSOCIATION_STATUS_ASSOCIATED;
        }
        else
        {
            object->associationStatus = DLMS_ASSOCIATION_STATUS_NON_ASSOCIATED;
            ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
        }
    }
    else if (e->index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctetString(e->parameters.byteArr, &object->secret);
#else
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
        uint32_t size = bb_available(e->parameters.byteArr);
#else
        uint16_t size = bb_available(e->parameters.byteArr);
#endif
        if (size == 0)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
        else
        {
            bb_clear(&object->secret);
            ret = bb_set(&object->secret, e->parameters.byteArr->data + e->parameters.byteArr->position, size);
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 5)
    {
#ifdef DLMS_IGNORE_MALLOC
        gxUser* it;
        if (!(object->userList.size < arr_getCapacity(&object->userList)))
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        else
        {
            ++object->userList.size;
            if ((ret = arr_getByIndex(&object->userList, object->userList.size - 1, (void**)&it, sizeof(gxUser))) == 0)
            {
                if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) != 0 ||
                    (ret = cosem_getUInt8(e->parameters.byteArr, &it->id)) != 0 ||
                    (ret = cosem_getString2(e->parameters.byteArr, it->name, sizeof(it->name))) != 0)
                {
                    //Error code is returned at the end of the function.
                }
            }
        }
#else
        if (e->parameters.vt != DLMS_DATA_TYPE_STRUCTURE || e->parameters.Arr->size != 2)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
        else
        {
            dlmsVARIANT* tmp;
            char* name;
            gxKey2* it = (gxKey2*)gxmalloc(sizeof(gxKey2));
            if ((ret = va_getByIndex(e->parameters.Arr, 0, &tmp)) != 0)
            {
                gxfree(it);
                return ret;
            }
            it->key = tmp->bVal;
            if ((ret = va_getByIndex(e->parameters.Arr, 1, &tmp)) != 0)
            {
                gxfree(it);
                return ret;
            }
            name = gxmalloc(tmp->strVal->size + 1);
            it->value = name;
            *(name + tmp->strVal->size) = '\0';
            memcpy(it->value, tmp->strVal->data, tmp->strVal->size);
            ret = arr_push(&((gxAssociationLogicalName*)e->target)->userList, it);
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 6)
    {
        int pos;
#ifdef DLMS_IGNORE_MALLOC
        gxUser* it;
        unsigned char id;
        char name[MAX_SAP_ITEM_NAME_LENGTH];
        if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) == 0 &&
            (ret = cosem_getUInt8(e->parameters.byteArr, &id)) == 0 &&
            (ret = cosem_getString2(e->parameters.byteArr, name, sizeof(name))) == 0)
        {
            uint16_t size = (uint16_t)strlen(name);
            for (pos = 0; pos != ((gxAssociationLogicalName*)e->target)->userList.size; ++pos)
            {
                if ((ret = arr_getByIndex(&((gxAssociationLogicalName*)e->target)->userList, pos, (void**)&it, sizeof(gxUser))) != 0)
                {
                    return ret;
                }
                if (it->id == id && memcmp(it->name, name, size) == 0)
                {
                    int pos2;
                    gxUser* it2;
                    for (pos2 = pos + 1; pos2 < ((gxAssociationLogicalName*)e->target)->userList.size; ++pos2)
                    {
                        if ((ret = arr_getByIndex(&((gxAssociationLogicalName*)e->target)->userList, pos2, (void**)&it2, sizeof(gxUser))) != 0)
                        {
                            break;
                        }
                        memcpy(it, it2, sizeof(gxUser));
                        it = it2;
                    }
                    --((gxAssociationLogicalName*)e->target)->userList.size;
                    break;
                }
            }
        }
#else
        if (e->parameters.Arr->size != 2)
        {
            return DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
        else
        {
            gxKey2* it;
            unsigned char id;
            dlmsVARIANT* tmp;
            unsigned char* name;
            ret = va_getByIndex(e->parameters.Arr, 0, &tmp);
            id = tmp->bVal;
            ret = va_getByIndex(e->parameters.Arr, 1, &tmp);
            name = tmp->strVal->data;
            int len = tmp->strVal->size;
            for (pos = 0; pos != ((gxAssociationLogicalName*)e->target)->userList.size; ++pos)
            {
                ret = arr_getByIndex(&((gxAssociationLogicalName*)e->target)->userList, pos, (void**)&it);
                if (ret != 0)
                {
                    return ret;
                }
                if (it->key == id && memcmp(it->value, name, len) == 0)
                {
                    ret = arr_removeByIndex(&((gxAssociationLogicalName*)e->target)->userList, pos, (void**)&it);
                    gxfree(it->value);
                    gxfree(it);
                    break;
                }
            }
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else
    {
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
    }
    return ret;
}
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME

#ifndef DLMS_IGNORE_IMAGE_TRANSFER
int invoke_ImageTransfer(
    gxImageTransfer* target,
    gxValueEventArg* e)
{
    int pos, ret = 0;
    //Image transfer initiate
    if (e->index == 1)
    {
        gxByteBuffer tmp;
        gxImageActivateInfo* it, * item = NULL;
        target->imageFirstNotTransferredBlockNumber = 0;
        ba_clear(&target->imageTransferredBlocksStatus);
        dlmsVARIANT* imageIdentifier, * size;
#ifdef DLMS_IGNORE_MALLOC
        dlmsVARIANT imageIdentifier2, size2;
        uint16_t count;
        unsigned char ch;
        if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) != 0 ||
            (ret = bb_getUInt8(e->parameters.byteArr, &ch)) != 0 ||
            ch != DLMS_DATA_TYPE_OCTET_STRING ||
            (ret = hlp_getObjectCount2(e->parameters.byteArr, &count)) != 0)
        {
            e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
            return ret;
        }
        imageIdentifier2.vt = DLMS_DATA_TYPE_OCTET_STRING;
        size2.vt = DLMS_DATA_TYPE_UINT32;
        //Attach image identifier so heap is not used.
        gxByteBuffer bb;
        bb_attach(&bb, e->parameters.byteArr->data + e->parameters.byteArr->position, count, count);
        imageIdentifier2.byteArr = &bb;
        e->parameters.byteArr->position += count;
        if ((ret = cosem_getUInt32(e->parameters.byteArr, &size2.ulVal)) != 0)
        {
            e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
            return ret;
        }
        imageIdentifier = &imageIdentifier2;
        size = &size2;
#else
        if ((ret = va_getByIndex(e->parameters.Arr, 0, &imageIdentifier)) != 0 ||
            (ret = va_getByIndex(e->parameters.Arr, 1, &size)) != 0)
        {
            e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
            return ret;
        }
#endif //DLMS_IGNORE_MALLOC
        target->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_INITIATED;
        unsigned char exists = 0;
        for (pos = 0; pos != target->imageActivateInfo.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&target->imageActivateInfo, pos, (void**)&it, sizeof(gxImageActivateInfo))) != 0)
            {
                return ret;
            }
#else
            if ((ret = arr_getByIndex(&target->imageActivateInfo, pos, (void**)&it)) != 0)
            {
                return ret;
            }
#endif //DLMS_IGNORE_MALLOC
            bb_attach(&tmp, it->identification.data, it->identification.size, it->identification.size);
            if (bb_size(imageIdentifier->byteArr) != 0 &&
                bb_compare(&tmp, imageIdentifier->byteArr->data, imageIdentifier->byteArr->size))
            {
                item = it;
                exists = 1;
            }
        }
        if (!exists)
        {
#ifdef DLMS_IGNORE_MALLOC
            ++target->imageActivateInfo.size;
            if ((ret = arr_getByIndex(&target->imageActivateInfo, pos, (void**)&item, sizeof(gxImageActivateInfo))) != 0)
            {
                --target->imageActivateInfo.size;
                return ret;
            }
#else
            item = (gxImageActivateInfo*)gxmalloc(sizeof(gxImageActivateInfo));
            BYTE_BUFFER_INIT(&item->identification);
            BYTE_BUFFER_INIT(&item->signature);
            arr_push(&target->imageActivateInfo, item);
#endif //DLMS_IGNORE_MALLOC
            item->size = var_toInteger(size);
#ifdef DLMS_IGNORE_MALLOC
            item->identification.size = (uint16_t)imageIdentifier->byteArr->size;
            if ((ret = bb_get(imageIdentifier->byteArr, item->identification.data, imageIdentifier->byteArr->size)) != 0)
            {
                return ret;
            }
#else
            item->identification.size = 0;
            if ((ret = bb_set2(&item->identification, imageIdentifier->byteArr, 0, bb_size(imageIdentifier->byteArr))) != 0)
            {
                return ret;
            }
#endif //DLMS_IGNORE_MALLOC
            uint16_t cnt = (uint16_t)(item->size / target->imageBlockSize);
            if (item->size % target->imageBlockSize != 0)
            {
                ++cnt;
            }
            cnt = ba_getByteCount(cnt);
#ifndef GX_DLMS_MICROCONTROLLER
            target->imageTransferredBlocksStatus.position = 0;
#endif //GX_DLMS_MICROCONTROLLER
#ifdef DLMS_IGNORE_MALLOC
            //Image transferred blocks status must handle in server side when malloc is not used if image size is huge.
            if (target->imageTransferredBlocksStatus.capacity != 0)
            {
                if (cnt > ba_getCapacity(&target->imageTransferredBlocksStatus))
                {
                    e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
                    return ret;
                }
                else
                {
                    target->imageTransferredBlocksStatus.size = 0;
                    if ((ret = ba_capacity(&target->imageTransferredBlocksStatus, cnt)) == 0)
                    {
                        for (pos = 0; pos != cnt; ++pos)
                        {
                            ba_set(&target->imageTransferredBlocksStatus, 0);
                        }
                    }
                }
            }
#else
            target->imageTransferredBlocksStatus.size = 0;
            if ((ret = ba_capacity(&target->imageTransferredBlocksStatus, cnt)) == 0)
            {
                for (pos = 0; pos != cnt; ++pos)
                {
                    ba_set(&target->imageTransferredBlocksStatus, 0);
                }
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    //Image block transfer
    else if (e->index == 2)
    {
        uint32_t index;
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) != 0 ||
            (ret = cosem_getUInt32(e->parameters.byteArr, &index)) != 0)
        {
            e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
            return ret;
        }
#else
        dlmsVARIANT* imageIndex;
        if ((ret = va_getByIndex(e->parameters.Arr, 0, &imageIndex)) != 0)
        {
            e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
            return ret;
        }
        index = (uint16_t)var_toInteger(imageIndex);
#endif //DLMS_IGNORE_MALLOC
        if ((ret = ba_setByIndex(&target->imageTransferredBlocksStatus, (uint16_t)index, 1)) == 0)
        {
            target->imageFirstNotTransferredBlockNumber = index + 1;
            target->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_INITIATED;
        }
#ifdef DLMS_IGNORE_MALLOC
        bb_clear(e->parameters.byteArr);
#endif //DLMS_IGNORE_MALLOC
    }
    //Image verify
    else if (e->index == 3)
    {
        ret = 0;
    }
    //Image activate.
    else if (e->index == 4)
    {
        ret = 0;
    }
    else
    {
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
    }
    return ret;
}
#endif //DLMS_IGNORE_IMAGE_TRANSFER

#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
int invoke_SapAssigment(
    gxSapAssignment* target,
    gxValueEventArg* e)
{
    int pos, ret = 0;
    uint16_t id;
    gxSapItem* it;
    //Image transfer initiate
    if (e->index == 1)
    {
#ifdef DLMS_IGNORE_MALLOC
        if (e->parameters.vt != DLMS_DATA_TYPE_OCTET_STRING)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
        else if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) == 0 &&
            (ret = cosem_getUInt16(e->parameters.byteArr, &id)) == 0)
        {
            if (id == 0)
            {
                uint16_t size;
                unsigned char name[MAX_SAP_ITEM_NAME_LENGTH];
                if ((ret = cosem_getOctetString2(e->parameters.byteArr, name, sizeof(name), &size)) == 0)
                {
                    name[size] = 0;
                    for (pos = 0; pos != target->sapAssignmentList.size; ++pos)
                    {
                        if ((ret = arr_getByIndex(&target->sapAssignmentList, pos, (void**)&it, sizeof(gxSapItem))) != 0)
                        {
                            return ret;
                        }
                        if (it->name.size == size && memcmp(it->name.value, name, size) == 0)
                        {
                            if ((ret = arr_removeByIndex(&target->sapAssignmentList, pos, sizeof(gxSapItem))) == 0)
                            {
                                //arr_removeByIndex is decreasing amount already.
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                //If buffer is full.
                if (!(target->sapAssignmentList.size < target->sapAssignmentList.capacity))
                {
                    ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                }
                else
                {
                    ++target->sapAssignmentList.size;
                    if ((ret = arr_getByIndex(&target->sapAssignmentList, target->sapAssignmentList.size - 1, (void**)&it, sizeof(gxSapItem))) == 0 &&
                        (ret = cosem_getOctetString2(e->parameters.byteArr, it->name.value, sizeof(it->name.value), &it->name.size)) == 0)
                    {
                        it->id = id;
                    }
                    else
                    {
                        --target->sapAssignmentList.size;
                    }
                }
            }
        }
#else
        dlmsVARIANT* tmp;
        if ((ret = va_getByIndex(e->parameters.Arr, 0, &tmp)) != 0)
        {
            return ret;
        }
        id = tmp->uiVal;
        if ((ret = va_getByIndex(e->parameters.Arr, 1, &tmp)) != 0)
        {
            return ret;
        }

        gxByteBuffer* name;
        if (tmp->vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            name = tmp->byteArr;
        }
        else
        {
            name = tmp->strVal;
        }
        if (id == 0)
        {
            for (pos = 0; pos != target->sapAssignmentList.size; ++pos)
            {
                if ((ret = arr_getByIndex(&target->sapAssignmentList, pos, (void**)&it)) != 0)
                {
                    return ret;
                }
                it->name.position = 0;
                if (name != NULL && bb_compare(&it->name, name->data, bb_size(name)))
                {
                    ret = arr_removeByIndex(&target->sapAssignmentList, pos, (void**)&it);
                    bb_clear(&it->name);
                    gxfree(it);
                    return ret;
                }
            }
        }
        else
        {
            it = (gxSapItem*)gxmalloc(sizeof(gxSapItem));
            it->id = id;
            BYTE_BUFFER_INIT(&it->name);
            bb_set(&it->name, name->data, name->size);
            arr_push(&target->sapAssignmentList, it);
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else
    {
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
    }
    return ret;
}
#endif //DLMS_IGNORE_SAP_ASSIGNMENT

#ifndef DLMS_IGNORE_SECURITY_SETUP
int invoke_SecuritySetup(dlmsServerSettings* settings, gxSecuritySetup* target, gxValueEventArg* e)
{
    int ret = 0;
#ifndef DLMS_IGNORE_HIGH_GMAC
    int pos;
#endif //DLMS_IGNORE_HIGH_GMAC
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
    gxCertificateInfo* it;
#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
    gxByteBuffer sn2;
    BYTE_BUFFER_INIT(&sn2);
#ifdef DLMS_IGNORE_MALLOC
    gxByteBuffer sn, issuer;
    BYTE_BUFFER_INIT(&sn);
    BYTE_BUFFER_INIT(&issuer);
#endif //DLMS_IGNORE_MALLOC
    if (target->securitySuite == 0 && e->index > 3)
    {
        ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
    }
    else if (e->index == 1)
    {
        //The security policy can only be strengthened.
        if (target->securityPolicy > var_toInteger(&e->parameters))
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
        else
        {
            target->securityPolicy = (DLMS_SECURITY_POLICY) var_toInteger(&e->parameters);
        }
    }
    else if (e->index == 2)
    {
#ifdef DLMS_IGNORE_HIGH_GMAC
        ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
#else
#ifdef DLMS_IGNORE_MALLOC
        if (e->parameters.vt != DLMS_DATA_TYPE_OCTET_STRING)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
        else
        {
#ifdef DLMS_IGNORE_MALLOC
            gxByteBuffer kek;
            BB_ATTACH(kek, settings->base.kek,
                settings->base.cipher.suite != DLMS_SECURITY_SUITE_V2 ? 16 : 32);
#endif //DLMS_IGNORE_MALLOC
            unsigned char TMP[24];
            gxByteBuffer tmp;
            BB_ATTACH(tmp, TMP, 0);
            unsigned char BUFF[24];
            gxByteBuffer bb;
            BB_ATTACH(bb, BUFF, 0);
            unsigned  char type;
            uint16_t count = 10;
            if ((ret = cosem_checkArray(e->parameters.byteArr, &count)) == 0)
            {
                for (pos = 0; pos != count; ++pos)
                {
                    if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) != 0 ||
                        (ret = cosem_getEnum(e->parameters.byteArr, &type)) != 0 ||
                        (ret = cosem_getOctetString(e->parameters.byteArr, &tmp)) != 0 ||
#ifdef DLMS_IGNORE_MALLOC
                        (ret = cip_decryptKey(&kek, &tmp, &bb)) != 0)
#else
                        (ret = cip_decryptKey(&settings->base.kek, &tmp, &bb)) != 0)
#endif //DLMS_IGNORE_MALLOC
                    {
                        break;
                    }
                    if ((target->securitySuite == DLMS_SECURITY_SUITE_V2 && bb.size != 32) ||
                        (target->securitySuite != DLMS_SECURITY_SUITE_V2 && bb.size != 16))
                    {
                        e->error = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                        break;
                    }
                    switch (type)
                    {
                    case DLMS_GLOBAL_KEY_TYPE_UNICAST_ENCRYPTION:
                        memcpy(target->guek, BUFF, bb.size);
                        break;
                    case DLMS_GLOBAL_KEY_TYPE_BROADCAST_ENCRYPTION:
                        memcpy(target->gbek, BUFF, bb.size);
                        break;
                    case DLMS_GLOBAL_KEY_TYPE_AUTHENTICATION:
                        memcpy(target->gak, BUFF, bb.size);
                        break;
                    case DLMS_GLOBAL_KEY_TYPE_KEK:
                        memcpy(settings->base.kek, BUFF, bb.size);
                        break;
                    default:
                        //Invalid type
                        ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                        break;
                    }
                }
            }
        }
#else
        dlmsVARIANT* it, * type, * data;
        if (e->parameters.vt != DLMS_DATA_TYPE_ARRAY)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
        else
        {
            gxByteBuffer bb;
            BYTE_BUFFER_INIT(&bb);
            for (pos = 0; pos != e->parameters.Arr->size; ++pos)
            {
                bb_clear(&bb);
                if ((ret = va_getByIndex(e->parameters.Arr, pos, &it)) != 0 ||
                    (ret = va_getByIndex(it->Arr, 0, &type)) != 0 ||
                    (ret = va_getByIndex(it->Arr, 1, &data)) != 0 ||
                    (ret = cip_decryptKey(&settings->base.kek, data->byteArr, &bb)) != 0)
                {
                    break;
                }
                if ((target->securitySuite == DLMS_SECURITY_SUITE_V2 && bb.size != 32) ||
                    (target->securitySuite != DLMS_SECURITY_SUITE_V2 && bb.size != 16))
                {
                    e->error = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    break;
                }
                switch (type->cVal)
                {
                case DLMS_GLOBAL_KEY_TYPE_UNICAST_ENCRYPTION:
                    bb_clear(&target->guek);
                    bb_set(&target->guek, bb.data, bb.size);
                    break;
                case DLMS_GLOBAL_KEY_TYPE_BROADCAST_ENCRYPTION:
                    bb_clear(&target->gbek);
                    bb_set(&target->gbek, bb.data, bb.size);
                    break;
                case DLMS_GLOBAL_KEY_TYPE_AUTHENTICATION:
                    bb_clear(&target->gak);
                    bb_set(&target->gak, bb.data, bb.size);
                    break;
                case DLMS_GLOBAL_KEY_TYPE_KEK:
                    bb_clear(&settings->base.kek);
                    bb_set(&settings->base.kek, bb.data, bb.size);
                    break;
                default:
                    //Invalid type
                    ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    break;
                }
            }
            bb_clear(&bb);
        }
#endif //DLMS_IGNORE_MALLOC
#endif //DLMS_IGNORE_HIGH_GMAC
    }
    else if (e->index == 3)
    {
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
    }
    else if (e->index == 4)
    {
        //Generate key pair.
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
        uint8_t type;
#ifdef DLMS_IGNORE_MALLOC
        if (e->parameters.vt != DLMS_DATA_TYPE_ENUM)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
        else
        {
            type = e->parameters.bVal;
        }
#else
        if (e->parameters.vt != DLMS_DATA_TYPE_ENUM)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
        else
        {
            type = (uint8_t)var_toInteger(&e->parameters);
        }
#endif //DLMS_IGNORE_MALLOC
        if (ret == 0)
        {
            gxPrivateKey* key;
            if (type == DLMS_CERTIFICATE_TYPE_DIGITAL_SIGNATURE)
            {
                key = &target->signingKey;
            }
            else if (type == DLMS_CERTIFICATE_TYPE_KEY_AGREEMENT)
            {
                key = &target->keyAgreementKey;
            }
            else if (type == DLMS_CERTIFICATE_TYPE_TLS)
            {
                key = &target->tlsKey;
            }
            else
            {
                ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
            }
            if (ret == 0)
            {
                ret = gxecdsa_generateKeyPair((ECC)(target->securitySuite - 1), &key->publicKey, key);
            }
        }
#else
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
    }
    else if (e->index == 5)
    {
        //GenerateCertificateRequest
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
        //Generate key pair.
        uint8_t type;
#ifdef DLMS_IGNORE_MALLOC
        if (e->parameters.vt != DLMS_DATA_TYPE_ENUM)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
        else
        {
            type = e->parameters.bVal;
        }
#else
        if (e->parameters.vt != DLMS_DATA_TYPE_ENUM)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
        else
        {
            type = (uint8_t)var_toInteger(&e->parameters);
        }
#endif //DLMS_IGNORE_MALLOC
        if (ret == 0)
        {
            gxPrivateKey* key;
            if (type == DLMS_CERTIFICATE_TYPE_DIGITAL_SIGNATURE)
            {
                key = &target->signingKey;
            }
            else if (type == DLMS_CERTIFICATE_TYPE_KEY_AGREEMENT)
            {
                key = &target->keyAgreementKey;
            }
            else if (type == DLMS_CERTIFICATE_TYPE_TLS)
            {
                key = &target->tlsKey;
            }
            else
            {
                ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
            }
            if (ret == 0)
            {
                if (key->rawValue.size != 0)
                {
                    e->byteArray = 1;
                    bb_empty(&settings->info.data);
                    if (key->publicKey.rawValue.size == 0)
                    {
                        ret = priv_getPublicKey(key, &key->publicKey);
                    }
                    if (ret == 0)
                    {
                        ret = pkcs10_createCertificateSigningRequest(key,
                            target->serverSystemTitle.data, &settings->info.data);
                        if (ret == 0 && settings->info.data.size != 0)
                        {
                            uint16_t size = (uint16_t)settings->info.data.size;
                            unsigned char cnt = hlp_getObjectCountSizeInBytes(size);
                            ret = bb_move(&settings->info.data, 0, cnt + 1, size);
                            settings->info.data.size = 0;
                            if ((ret = bb_setUInt8(&settings->info.data, DLMS_DATA_TYPE_OCTET_STRING)) == 0 &&
                                (ret = hlp_setObjectCount(size, &settings->info.data)) == 0)
                            {
                                settings->info.data.size += size;
                            }
                        }
                    }
                    else
                    {
                        ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    }
                }
            }
        }
#else
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
    }
    else if (e->index == 6)
    {
        //Import certificate.
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
#ifdef DLMS_IGNORE_MALLOC
        if (e->parameters.vt != DLMS_DATA_TYPE_OCTET_STRING)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
#else
        if (e->parameters.vt != DLMS_DATA_TYPE_OCTET_STRING)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
#endif //DLMS_IGNORE_MALLOC
        if (ret == 0)
        {
#ifdef DLMS_IGNORE_MALLOC
            unsigned char ch;
            uint16_t count;
            gxCertificateInfo* it;
            //Find by system title and key usage.
            if ((ret = bb_getUInt8(e->parameters.byteArr, &ch)) != 0 ||
                ch != DLMS_DATA_TYPE_OCTET_STRING ||
                hlp_getObjectCount2(e->parameters.byteArr, &count) != 0 ||
                bb_available(e->parameters.byteArr) < count)
            {
                return DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
            }
            if ((ret = gx509Certificate_getSystemTitle(e->parameters.byteArr, &sn)) == 0)
            {
                //Remove old server certificate.
                for (pos = 0; pos != target->certificates.size; ++pos)
                {
                    if ((ret = arr_getByIndex(&target->certificates, target->certificates.size - 1, (void**)&it, sizeof(gxCertificateInfo))) != 0)
                    {
                        return DLMS_ERROR_CODE_READ_WRITE_DENIED;
                    }
                    if ((ret = gx509Certificate_getSystemTitle(&it->cert, &sn2)) != 0)
                    {
                        break;
                    }
                    if (memcmp(sn2.data, sn.data, 8) == 0)
                    {
                        ret = arr_removeByIndex(&target->certificates, pos, sizeof(gxCertificateInfo));
                        break;
                    }
                }
                if (ret == 0)
                {
                    if (!(target->certificates.size < target->certificates.capacity))
                    {
                        //If certificates are full.
                        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
                    }
                    else
                    {
                        ++target->certificates.size;
                        if ((ret = arr_getByIndex(&target->certificates, target->certificates.size - 1, (void**)&it, sizeof(gxCertificateInfo))) != 0)
                        {
                            --target->certificates.size;
                            ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
                        }
                        else
                        {
                            //Update the new certificate.
                            bb_clear(&it->cert);
                            if ((ret = bb_set(&it->cert,
                                e->parameters.byteArr->data + e->parameters.byteArr->position,
                                bb_available(e->parameters.byteArr))) != 0)
                            {
                                --target->certificates.size;
                            }
                        }
                    }
                }
                bb_clear(e->parameters.byteArr);
            }
#else
            gx509Certificate cert;
            gx509Certificate certInstalled;
            gxCertificateInfo* it;
            gxCertificateInfo* cInfo = (gxCertificateInfo*)gxmalloc(sizeof(gxCertificateInfo));
            if (cInfo == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            BYTE_BUFFER_INIT(&cInfo->cert);
            if ((ret = bb_set(&cInfo->cert, e->parameters.byteArr->data, e->parameters.byteArr->size)) == 0)
            {
                gx509Certificate_init(&cert);
                gx509Certificate_init(&certInstalled);
                uint32_t now = 0;
                if (settings->defaultClock != NULL)
                {
                    now = time_toUnixTime2(&settings->defaultClock->time);
                }
                if ((ret = gx509Certificate_fromBytes(&cert, DLMS_X509_CERTIFICATE_DATA_NONE, e->parameters.byteArr)) == 0 &&
                    (ret = gx509Certificate_validate(&cert, now)) == 0)
                {
                    //Remove old server certificate.
                    for (pos = 0; pos != target->certificates.size; ++pos)
                    {
                        if ((ret = arr_getByIndex(&target->certificates, pos, (void**)&it)) != 0)
                        {
                            break;
                        }
                        if ((ret = gx509Certificate_fromBytes(&certInstalled,
                            DLMS_X509_CERTIFICATE_DATA_NONE, &it->cert)) == 0)
                        {
                            if (certInstalled.keyUsage == cert.keyUsage &&
                                memcmp(certInstalled.systemTitle, cert.systemTitle, 8) == 0)
                            {
                                if ((ret = arr_removeByIndex(&target->certificates, pos, (void**)&it)) == 0)
                                {
                                    gxfree(it);
                                }
                                break;
                            }
                        }
                    }
                    if (ret == 0)
                    {
                        ret = arr_push(&target->certificates, cInfo);
                    }
                }
            }
            if (ret != 0 && cInfo != NULL)
            {
                gxfree(cInfo);
            }
#endif //DLMS_IGNORE_MALLOC
        }
#else
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
    }
    else if (e->index == 7)
    {
        //Export certificate.
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
        uint8_t type;
#ifdef DLMS_IGNORE_MALLOC
        if (e->parameters.vt != DLMS_DATA_TYPE_OCTET_STRING)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
        else
        {
            if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) == 0 &&
                (ret = cosem_getEnum(e->parameters.byteArr, &type)) == 0)
            {
            }
        }
#else
        dlmsVARIANT* sn, * tmp, * tmp2;
        if ((ret = va_getByIndex(e->parameters.Arr, 0, &tmp)) == 0)
        {
            type = (uint8_t)var_toInteger(tmp);
        }
#endif //DLMS_IGNORE_MALLOC
        if (ret == 0)
        {
            if (type == 0)
            {
                //Find certificate by entity.
            }
            else if (type == 1)
            {
                //Find by serial.
#ifdef DLMS_IGNORE_MALLOC
                unsigned char ch;
                uint16_t count;
                if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) == 0 &&
                    //serialNumber
                    (ret = bb_getUInt8(e->parameters.byteArr, &ch)) == 0 &&
                    ch == DLMS_DATA_TYPE_OCTET_STRING &&
                    (ret = hlp_getObjectCount2(e->parameters.byteArr, &count)) == 0)
                {
                    bb_attach(&sn, e->parameters.byteArr->data + e->parameters.byteArr->position, count, count);
                    e->parameters.byteArr->position += count;
                    //Issuer
                    if ((ret = bb_getUInt8(e->parameters.byteArr, &ch)) == 0 &&
                        ch == DLMS_DATA_TYPE_OCTET_STRING &&
                        (ret = hlp_getObjectCount2(e->parameters.byteArr, &count)) == 0)
                    {
                        bb_attach(&issuer, e->parameters.byteArr->data + e->parameters.byteArr->position, count, count);
                        e->parameters.byteArr->position += count;
                        //Find certificate.
                        for (pos = 0; pos != target->certificates.size; ++pos)
                        {
                            if ((ret = arr_getByIndex(&target->certificates, pos, (void**)&it, sizeof(gxCertificateInfo))) != 0)
                            {
                                break;
                            }
                            if ((ret = gx509Certificate_getSerialNumber(&it->cert, &sn2)) != 0)
                            {
                                break;
                            }
                            if (bb_size(&sn) == bb_size(&sn2) &&
                                bb_compare(&sn, sn2.data, sn2.size))
                            {
                                e->byteArray = 1;
                                bb_empty(&settings->info.data);
                                if ((ret = bb_setUInt8(&settings->info.data, DLMS_DATA_TYPE_OCTET_STRING)) == 0 &&
                                    (ret = hlp_setObjectCount(it->cert.size, &settings->info.data)) == 0 &&
                                    (ret = bb_set(&settings->info.data, it->cert.data, it->cert.size)) == 0)
                                {
                                }
                                return ret;
                            }
                        }
                        //Return error if certificate is not found.
                        ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    }
                }
            }
#else
                if ((ret = va_getByIndex(e->parameters.Arr, 1, &tmp2)) == 0 &&
                    //serialNumber
                    (ret = va_getByIndex(tmp2->Arr, 0, &sn)) == 0 &&
                    //Issuer
                    (ret = va_getByIndex(tmp2->Arr, 1, &tmp)) == 0)
                {
                    //Find certificate.
                    for (pos = 0; pos != target->certificates.size; ++pos)
                    {
                        if ((ret = arr_getByIndex(&target->certificates, pos, (void**)&it)) != 0)
                        {
                            break;
                        }
                        if ((ret = gx509Certificate_getSerialNumber(&it->cert, &sn2)) != 0)
                        {
                            break;
                        }
                        if (bb_size(sn->byteArr) == bb_size(&sn2) &&
                            bb_compare(sn->byteArr, sn2.data, sn2.size))
                        {
                            e->byteArray = 1;
                            bb_empty(&settings->info.data);
                            if ((ret = bb_setUInt8(&settings->info.data, DLMS_DATA_TYPE_OCTET_STRING)) == 0 &&
                                (ret = hlp_setObjectCount(it->cert.size, &settings->info.data)) == 0 &&
                                (ret = bb_set(&settings->info.data, it->cert.data, it->cert.size)) == 0)
                            {
                            }
                            return ret;
                        }
                    }
                    //Return error if certificate is not found.
                    ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                }
        }
#endif //DLMS_IGNORE_MALLOC
            else
            {
                ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
            }
    }
#else
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
    }
    else if (e->index == 8)
    {
        //Remove certificate.
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
        uint8_t type;
#ifdef DLMS_IGNORE_MALLOC
        if (e->parameters.vt != DLMS_DATA_TYPE_OCTET_STRING)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
        else
        {
            if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) == 0 &&
                (ret = cosem_getEnum(e->parameters.byteArr, &type)) == 0)
            {
            }
        }
#else
        dlmsVARIANT* sn, * tmp, * tmp2;
        if ((ret = va_getByIndex(e->parameters.Arr, 0, &tmp)) == 0)
        {
            type = (uint8_t)var_toInteger(tmp);
        }
#endif //DLMS_IGNORE_MALLOC
        if (ret == 0)
        {
            if (type == 0)
            {
                //Find certificate by entity.
            }
            else if (type == 1)
            {
                gxCertificateInfo* it;
#ifdef DLMS_IGNORE_MALLOC
                unsigned char ch;
                uint16_t count;
                if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) == 0 &&
                    //serialNumber
                    (ret = bb_getUInt8(e->parameters.byteArr, &ch)) == 0 &&
                    ch == DLMS_DATA_TYPE_OCTET_STRING &&
                    (ret = hlp_getObjectCount2(e->parameters.byteArr, &count)) == 0)
                {
                    bb_attach(&sn, e->parameters.byteArr->data + e->parameters.byteArr->position, count, count);
                    e->parameters.byteArr->position += count;
                    //Issuer
                    if ((ret = bb_getUInt8(e->parameters.byteArr, &ch)) == 0 &&
                        ch == DLMS_DATA_TYPE_OCTET_STRING &&
                        (ret = hlp_getObjectCount2(e->parameters.byteArr, &count)) == 0)
                    {
                        bb_attach(&issuer, e->parameters.byteArr->data + e->parameters.byteArr->position, count, count);
                        e->parameters.byteArr->position += count;
                        //Find certificate.
                        for (pos = 0; pos != target->certificates.size; ++pos)
                        {
                            if ((ret = arr_getByIndex(&target->certificates, pos, (void**)&it, sizeof(gxCertificateInfo))) != 0)
                            {
                                break;
                            }
                            if ((ret = gx509Certificate_getSerialNumber(&it->cert, &sn2)) != 0)
                            {
                                break;
                            }
                            if (bb_size(&sn) == bb_size(&sn2) &&
                                bb_compare(&sn, sn2.data, sn2.size))
                            {
                                bb_empty(&settings->info.data);
                                return arr_removeByIndex(&target->certificates, pos, sizeof(gxCertificateInfo));
                            }
                        }
                        //Return error if certificate is not found.
                        ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    }
                }
            }
#else
                gxByteBuffer sn2;
                if ((ret = va_getByIndex(e->parameters.Arr, 1, &tmp2)) == 0 &&
                    //serialNumber
                    (ret = va_getByIndex(tmp2->Arr, 0, &sn)) == 0 &&
                    //Issuer
                    (ret = va_getByIndex(tmp2->Arr, 1, &tmp)) == 0)
                {
                    //Find certificate.
                    for (pos = 0; pos != target->certificates.size; ++pos)
                    {
                        if ((ret = arr_getByIndex(&target->certificates, pos, (void**)&it)) != 0)
                        {
                            break;
                        }
                        if ((ret = gx509Certificate_getSerialNumber(&it->cert, &sn2)) != 0)
                        {
                            break;
                        }
                        if (bb_size(sn->byteArr) == bb_size(&sn2) &&
                            bb_compare(sn->byteArr, sn2.data, sn2.size))
                        {
                            bb_empty(&settings->info.data);
                            if ((ret = arr_removeByIndex(&target->certificates, pos, (void**)&it)) == 0)
                            {
                                gxfree(it);
                            }
                            return ret;
                        }
                    }
                    //Return error if certificate is not found.
                    ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                }
        }
#endif //DLMS_IGNORE_MALLOC
            else
            {
                ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
            }
    }
#else
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
    }
    else
    {
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
    }
    return ret;
}

#endif //DLMS_IGNORE_SECURITY_SETUP

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int invoke_AssociationShortName(
    dlmsServerSettings* settings,
    gxValueEventArg* e)
{
    int ret;
    unsigned char equal;
    uint32_t ic = 0;
#ifndef DLMS_IGNORE_HIGH_GMAC
    gxByteBuffer bb;
#endif //DLMS_IGNORE_HIGH_GMAC
    gxByteBuffer* readSecret;
    // Check reply_to_HLS_authentication
    if (e->index == 8)
    {
#ifndef DLMS_IGNORE_HIGH_GMAC
        if (settings->base.authentication == DLMS_AUTHENTICATION_HIGH_GMAC)
        {
            unsigned char ch;
            bb_attach(&bb, settings->base.sourceSystemTitle, sizeof(settings->base.sourceSystemTitle), sizeof(settings->base.sourceSystemTitle));
            readSecret = &bb;
            if ((ret = bb_getUInt8(&settings->info.data, &ch)) != 0 ||
                (ret = bb_getUInt32(&settings->info.data, &ic)) != 0)
            {
                bb_clear(&settings->info.data);
                return ret;
            }
        }
        else
#endif //DLMS_IGNORE_HIGH_GMAC
        {
            readSecret = &((gxAssociationShortName*)e->target)->secret;
        }
        bb_clear(&settings->info.data);
        if ((ret = dlms_secure(&settings->base, ic,
            &settings->base.stoCChallenge, readSecret, &settings->info.data)) != 0)
        {
            bb_clear(&settings->info.data);
            return ret;
        }
        equal = bb_size(e->parameters.byteArr) != 0 &&
            bb_compare(&settings->info.data,
                e->parameters.byteArr->data,
                e->parameters.byteArr->size);
        bb_clear(&settings->info.data);
        if (equal)
        {
            e->byteArray = 1;
#ifndef DLMS_IGNORE_HIGH_GMAC
            if (settings->base.authentication == DLMS_AUTHENTICATION_HIGH_GMAC)
            {
#ifdef DLMS_IGNORE_MALLOC
                bb_attach(&bb, settings->base.cipher.systemTitle, sizeof(settings->base.cipher.systemTitle), sizeof(settings->base.cipher.systemTitle));
                readSecret = &bb;
#else
                readSecret = &settings->base.cipher.systemTitle;
#endif //DLMS_IGNORE_MALLOC
                ic = settings->base.cipher.invocationCounter;
            }
#endif //DLMS_IGNORE_HIGH_GMAC
            if ((ret = dlms_secure(&settings->base,
                ic,
                &settings->base.ctoSChallenge,
                readSecret,
                &settings->info.data)) != 0)
            {
                return ret;
            }
            bb_insertUInt8(&settings->info.data, 0, DLMS_DATA_TYPE_OCTET_STRING);
            bb_insertUInt8(&settings->info.data, 1, (unsigned char)(settings->info.data.size - 1));
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return DLMS_ERROR_CODE_READ_WRITE_DENIED;
    }
    return 0;
}
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_SCRIPT_TABLE
int invoke_ScriptTable(
    dlmsServerSettings* settings,
    gxValueEventArg* e)
{
    gxScript* s;
    gxScriptAction* sa;
    gxValueEventArg* e1;
    int ret = 0, pos, pos2;
    unsigned char id = var_toInteger(&e->parameters);
    //Find index and execute it.
    if (e->index == 1)
    {
        gxValueEventCollection args;
#ifdef DLMS_IGNORE_MALLOC
        gxValueEventArg tmp[1];
        ve_init(&tmp[0]);
        vec_attach(&args, tmp, 1, 1);
        e1 = &tmp[0];
#else
        e1 = (gxValueEventArg*)gxmalloc(sizeof(gxValueEventArg));
        ve_init(e1);
        vec_init(&args);
        vec_push(&args, e1);
#endif //DLMS_IGNORE_MALLOC
        for (pos = 0; pos != ((gxScriptTable*)e->target)->scripts.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&((gxScriptTable*)e->target)->scripts, pos, (void**)&s, sizeof(gxScript))) != 0)
#else
            if ((ret = arr_getByIndex(&((gxScriptTable*)e->target)->scripts, pos, (void**)&s)) != 0)
#endif //DLMS_IGNORE_MALLOC
            {
                break;
            }
            if (s->id == id)
            {
                for (pos2 = 0; pos2 != s->actions.size; ++pos2)
                {
#ifdef DLMS_IGNORE_MALLOC
                    if ((ret = arr_getByIndex(&s->actions, pos2, (void**)&sa, sizeof(gxScriptAction))) != 0)
#else
                    if ((ret = arr_getByIndex(&s->actions, pos2, (void**)&sa)) != 0)
#endif //DLMS_IGNORE_MALLOC
                    {
                        break;
                    }
                    if (sa->type == DLMS_SCRIPT_ACTION_TYPE_WRITE)
                    {
                        e1->value = sa->parameter;
                    }
                    else
                    {
                        e1->parameters = sa->parameter;
                    }
                    e1->index = sa->index;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    e1->target = sa->target;
#else
                    if ((ret = oa_findByLN(&settings->base.objects, sa->objectType, sa->logicalName, &e1->target)) != 0)
                    {
                        break;
                    }
                    if (e1->target == NULL)
                    {
                        ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                        break;
                    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    if (sa->type == DLMS_SCRIPT_ACTION_TYPE_WRITE)
                    {
                        svr_preWrite(&settings->base, &args);
                        if (e1->error != 0)
                        {
                            ret = e1->error;
                        }
                        else if (!e1->handled)
                        {
                            //Set action to true to indicate that this is called from the action method.
                            e1->action = 1;
                            if ((ret = cosem_setValue(&settings->base, e1)) != 0)
                            {
                                break;
                            }
                            svr_postWrite(&settings->base, &args);
                            if (e1->error != 0)
                            {
                                ret = e1->error;
                            }
                        }
                    }
                    else if (sa->type == DLMS_SCRIPT_ACTION_TYPE_EXECUTE)
                    {
                        svr_preAction(&settings->base, &args);
                        if (e1->error != 0)
                        {
                            ret = e1->error;
                        }
                        else if (!e1->handled)
                        {
                            if ((ret = cosem_invoke(settings, e1)) != 0)
                            {
                                break;
                            }
                            svr_postAction(&settings->base, &args);
                            if (e1->error != 0)
                            {
                                ret = e1->error;
                            }
                        }
                    }
                    else
                    {
                        ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    }
                    if (ret != 0)
                    {
                        break;
                    }
                }
                //Stop looping once the script ID is found, since it is unique.
                break;
            }
        }
        vec_clear(&args);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
    }
    var_clear(&e->value);
    return ret;
}
#endif //DLMS_IGNORE_SCRIPT_TABLE

#ifndef DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
int invoke_zigbeeNetworkControl(gxZigBeeNetworkControl* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = 0, pos;
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* it;
#endif //DLMS_IGNORE_MALLOC
    gxActiveDevice* ad;
    uint32_t v;
    //Register device.
    if (index == 1)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(&object->activeDevices, object->activeDevices.size, (void**)&ad, sizeof(gxActiveDevice))) == 0)
        {
            ++object->activeDevices.size;
            BYTE_BUFFER_INIT(&ad->macAddress);
            if ((ret = cosem_getOctetString(value->byteArr, &ad->macAddress)) == 0 &&
                (ret = cosem_getIntegerFromBitString(value->byteArr, &v)) == 0)
            {
                ad->status = (DLMS_ZIG_BEE_STATUS)v;
            }
        }
#else
        if ((ret = va_getByIndex(value->Arr, 0, &it)) == 0)
        {
            ad = (gxActiveDevice*)gxcalloc(1, sizeof(gxActiveDevice));
            BYTE_BUFFER_INIT(&ad->macAddress);
            if ((ret = bb_set(&ad->macAddress, it->byteArr->data, it->byteArr->size)) == 0 &&
                (ret = cosem_getIntegerFromBitString(it->byteArr, &v)) == 0)
            {
                ad->status = (DLMS_ZIG_BEE_STATUS)v;
                arr_push(&object->activeDevices, ad);
            }
            else
            {
                gxfree(ad);
            }
        }
#endif //DLMS_IGNORE_MALLOC
    }
    //Unregister device.
    else if (index == 2)
    {
        for (pos = 0; pos != object->activeDevices.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->activeDevices, pos, (void**)&ad, sizeof(gxActiveDevice))) != 0)
            {
                break;
            }
            if (memcpy(&ad->macAddress, &value->byteArr, 8) == 0)
            {
                int pos2;
                gxActiveDevice* ad2;
                for (pos2 = pos + 1; pos2 < object->activeDevices.size; ++pos2)
                {
                    if ((ret = arr_getByIndex(&object->activeDevices, pos2, (void**)&ad2, sizeof(gxActiveDevice))) != 0)
                    {
                        break;
                    }
                    memcpy(ad, ad2, sizeof(gxActiveDevice));
                    ad = ad2;
                }
                --object->activeDevices.size;
                break;
            }
#else
            ret = arr_getByIndex(&object->activeDevices, pos, (void**)&ad);
            if (ret != 0)
            {
                return ret;
            }
            if (memcpy(&ad->macAddress, &value->byteArr, 8) == 0)
            {
                ret = arr_removeByIndex(&object->activeDevices, pos, (void**)&ad);
                if (ret != 0)
                {
                    return ret;
                }
                gxfree(ad);
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    //Unregister all device.
    else if (index == 3)
    {
        ret = obj_clearActiveDevices(&object->activeDevices);
    }
    //backup PAN
    else if (index == 4)
    {
    }
    //Restore PAN,
    else if (index == 5)
    {
        // This method instructs the coordinator to restore a PAN using backup information.
        // The storage location of the back-up is not currently defined and is an internal function
        // of the DLMS/COSEM Server.
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
int invoke_ExtendedRegister(
    dlmsServerSettings* settings,
    gxExtendedRegister* object,
    unsigned char index)
{
    int ret = 0;
    //Reset.
    if (index == 1)
    {
        ret = var_clear(&object->value);
        if (ret != 0)
        {
            return ret;
        }
        if (settings->defaultClock != NULL)
        {
            object->captureTime = settings->defaultClock->time;
        }
        else
        {
            time_clear(&object->captureTime);
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_EXTENDED_REGISTER

#ifndef DLMS_IGNORE_DEMAND_REGISTER
int invoke_DemandRegister(
    dlmsServerSettings* settings,
    gxDemandRegister* object,
    unsigned char index)
{
    int ret = 0;
    //Reset.
    if (index == 1)
    {
        if ((ret = var_clear(&object->currentAverageValue)) != 0 ||
            (ret = var_clear(&object->lastAverageValue)) != 0)
        {
            return ret;
        }
        if (settings->defaultClock != NULL)
        {
            object->startTimeCurrent = object->captureTime = settings->defaultClock->time;
        }
        else
        {
            time_clear(&object->captureTime);
            time_clear(&object->startTimeCurrent);
        }
    }
    else if (index == 1)
    {
        if ((ret = var_copy(&object->lastAverageValue, &object->currentAverageValue)) != 0 ||
            (ret = var_clear(&object->currentAverageValue)) != 0)
        {
            return ret;
        }
        if (settings->defaultClock != NULL)
        {
            object->startTimeCurrent = object->captureTime = settings->defaultClock->time;
        }
        else
        {
            time_clear(&object->captureTime);
            time_clear(&object->startTimeCurrent);
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_DEMAND_REGISTER


#ifndef DLMS_IGNORE_CLOCK
int invoke_Clock(gxClock* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* it;
    dlmsVARIANT tmp;
#endif //DLMS_IGNORE_MALLOC
    // Resets the value to the default value.
    // The default value is an instance specific constant.
    if (index == 1)
    {
#ifdef DLMS_USE_EPOCH_TIME
        int minutes = time_getMinutes(&object->time);
#else
        int minutes = object->time.value.tm_min;
#endif // DLMS_USE_EPOCH_TIME
        if (minutes < 8)
        {
            minutes = 0;
        }
        else if (minutes < 23)
        {
            minutes = 15;
        }
        else if (minutes < 38)
        {
            minutes = 30;
        }
        else if (minutes < 53)
        {
            minutes = 45;
        }
        else
        {
            minutes = 0;
            time_addHours(&object->time, 1);
        }
#ifdef DLMS_USE_EPOCH_TIME
        time_addTime(&object->time, 0, -time_getMinutes(&object->time) + minutes, -time_getSeconds(&object->time));
#else
        time_addTime(&object->time, 0, -object->time.value.tm_min + minutes, -object->time.value.tm_sec);
#endif // DLMS_USE_EPOCH_TIME
    }
    // Sets the meter's time to the nearest minute.
    else if (index == 3)
    {
#ifdef DLMS_USE_EPOCH_TIME
        int s = time_getSeconds(&object->time);
#else
        int s = object->time.value.tm_sec;
#endif // DLMS_USE_EPOCH_TIME
        if (s > 30)
        {
            time_addTime(&object->time, 0, 1, 0);
        }
        time_addTime(&object->time, 0, 0, -s);
    }
    //Adjust to preset time.
    else if (index == 4)
    {
        object->time = object->presetTime;
    }
    // Presets the time to a new value (preset_time) and defines
    // avalidity_interval within which the new time can be activated.
    else if (index == 5)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateFromOctetString(value->byteArr, &object->presetTime);
#else
        ret = var_init(&tmp);
        if (ret != 0)
        {
            return ret;
        }
        ret = va_getByIndex(value->Arr, 0, &it);
        if (ret != 0)
        {
            return ret;
        }
        ret = dlms_changeType2(it, DLMS_DATA_TYPE_DATETIME, &tmp);
        if (ret != 0)
        {
            return ret;
        }
        object->presetTime = *tmp.dateTime;
#endif //DLMS_IGNORE_MALLOC
    }
    // Shifts the time.
    else if (index == 6)
    {
        time_addTime(&object->time, 0, 0, var_toInteger(value));
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_REGISTER
int invoke_Register(
    gxRegister* object,
    gxValueEventArg* e)
{
    int ret;
    //Reset.
    if (e->index == 1)
    {
        if (e->parameters.vt != DLMS_DATA_TYPE_INT8 ||
            e->parameters.cVal != 0)
        {
            ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
        }
        else
        {
            ret = var_clear(&object->value);
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER

#ifndef DLMS_IGNORE_PROFILE_GENERIC
/*
* Copies the values of the objects to capture into the buffer by reading
* capture objects.
*/
int capture(dlmsSettings* settings,
    gxProfileGeneric* object)
{
#ifdef DLMS_IGNORE_MALLOC
    gxValueEventArg e;
    gxValueEventCollection args;
    ve_init(&e);
    e.action = 1;
    e.target = &object->base;
    e.index = 2;
    gxValueEventArg p[1] = { e };
    vec_attach(&args, p, 1, 1);
    svr_preGet(settings, &args);
    svr_postGet(settings, &args);
    vec_empty(&args);
#else
    int ret, pos;
    gxKey* kv;
    dlmsVARIANT* value;
    variantArray* row;
    gxValueEventArg e;
    gxValueEventCollection args;
    ve_init(&e);
    e.action = 1;
    e.target = &object->base;
    e.index = 2;
    vec_init(&args);
    vec_push(&args, &e);
    svr_preGet(settings, &args);
    if (!e.handled)
    {
        gxValueEventArg e2;
        ve_init(&e2);
        row = (variantArray*)gxmalloc(sizeof(variantArray));
        va_init(row);
        for (pos = 0; pos != object->captureObjects.size; ++pos)
        {
            ret = arr_getByIndex(&object->captureObjects, pos, (void**)&kv);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            e2.target = (gxObject*)kv->key;
            e2.index = ((gxTarget*)kv->value)->attributeIndex;
            if ((ret = cosem_getData(&e2)) != 0)
            {
                return ret;
            }
            value = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
            var_init(value);
            var_copy(value, &e.value);
            va_push(row, value);
        }
        // Remove first item if buffer is full.
        if (object->profileEntries == object->buffer.size)
        {
            variantArray* removedRow;
            arr_removeByIndex(&object->buffer, 0, (void**)&removedRow);
            va_clear(removedRow);
        }
        arr_push(&object->buffer, row);
        object->entriesInUse = object->buffer.size;
    }
    svr_postGet(settings, &args);
    vec_empty(&args);
#endif //DLMS_IGNORE_MALLOC
    return 0;
}

int invoke_ProfileGeneric(
    dlmsServerSettings* settings,
    gxProfileGeneric* object,
    unsigned char index)
{
    int ret = 0;
    //Reset.
    if (index == 1)
    {
#ifndef DLMS_IGNORE_MALLOC
        ret = obj_clearProfileGenericBuffer(&object->buffer);
#endif //DLMS_IGNORE_MALLOC
    }
    //Capture.
    else if (index == 2)
    {
        // Capture.
        ret = capture(&settings->base, object);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC


#ifndef DLMS_IGNORE_COMPACT_DATA
int compactDataAppend(unsigned char byteArray, dlmsVARIANT* value3, gxByteBuffer* bb)
{
#ifdef DLMS_IGNORE_MALLOC
    int ret;
    if (byteArray && value3->vt == DLMS_DATA_TYPE_OCTET_STRING)
    {
        if (bb_size(value3->byteArr) == 1)
        {
            ret = bb_setUInt8(bb, 0);
        }
        else
        {
            ret = bb_set(bb, value3->byteArr->data + 1, value3->byteArr->size - 1);
        }
        return ret;
    }
#endif //DLMS_IGNORE_MALLOC
    //Compact data adds count to array, bit-string.
    return var_getBytes4(value3, value3->vt, bb, 0, 1, 0);
}

int compactDataAppendArray(dlmsVARIANT* value, gxByteBuffer* bb, uint16_t dataIndex)
{
    int ret, pos;
    int cnt = value->Arr->size;
    if (dataIndex > 0xFFF)
    {
        //0x1000 to 0xFFFF is the selective access to the array.
        cnt = value->Arr->size;
    }
    dlmsVARIANT* value2;
    for (pos = 0; pos != cnt; ++pos)
    {
        if ((ret = va_getByIndex(value->Arr, pos, &value2)) != 0)
        {
            return ret;
        }
        if (value2->vt == DLMS_DATA_TYPE_STRUCTURE)
        {
            dlmsVARIANT* value3;
            int pos1;
            for (pos1 = 0; pos1 != value2->Arr->size; ++pos1)
            {
                if ((ret = va_getByIndex(value2->Arr, pos1, &value3)) != 0 ||
                    (ret = compactDataAppend(0, value3, bb)) != 0)
                {
                    return ret;
                }
            }
        }
        else
        {
            if ((ret = compactDataAppend(0, value2, bb)) != 0)
            {
                return ret;
            }
        }
    }
    return 0;
}

/*
* Copies the values of the objects to capture into the buffer by reading
* capture objects.
*/
int cosem_captureCompactData(
    dlmsSettings* settings,
    gxCompactData* object)
{
#ifdef DLMS_IGNORE_MALLOC
    return compactData_update(settings, object, 0);
#else
    int ret = 0;
    uint16_t pos;
    gxKey* kv;
    gxValueEventCollection args;
    bb_clear(&object->buffer);

    gxValueEventArg* e = (gxValueEventArg*)gxmalloc(sizeof(gxValueEventArg));
    ve_init(e);
    vec_init(&args);
    vec_push(&args, e);
    e->action = 1;
    e->target = &object->base;
    e->index = 2;
    //Allocate space where captured values are saved before they are added to the buffer.
    // We can't use server buffer because there might be transaction on progress when this is called.
    unsigned char tmp[MAX_CAPTURE_OBJECT_BUFFER_SIZE];
    gxByteBuffer bb;
    bb_attach(&bb, tmp, 0, sizeof(tmp));
    svr_preGet(settings, &args);
    if (!e->handled)
    {
        for (pos = 0; pos != object->captureObjects.size; ++pos)
        {
            ret = arr_getByIndex(&object->captureObjects, pos, (void**)&kv);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                bb_clear(&object->buffer);
                break;
            }
            e->target = (gxObject*)kv->key;
            e->index = ((gxTarget*)kv->value)->attributeIndex;
            e->dataIndex = ((gxTarget*)kv->value)->dataIndex;
            if ((ret = cosem_getValue(settings, e)) != 0)
            {
                bb_clear(&object->buffer);
                break;
            }
            if (e->byteArray && e->value.vt == DLMS_DATA_TYPE_OCTET_STRING)
            {
                gxDataInfo info;
                dlmsVARIANT value;
                di_init(&info);
                var_init(&value);
                if ((ret = dlms_getData(e->value.byteArr, &info, &value)) != 0)
                {
                    var_clear(&value);
                    break;
                }
                if (info.type == DLMS_DATA_TYPE_STRUCTURE ||
                    info.type == DLMS_DATA_TYPE_ARRAY)
                {
#ifdef DLMS_ITALIAN_STANDARD
                    //Italian standard requires that there is a array count in data.
                    if (info.type == DLMS_DATA_TYPE_ARRAY)
                    {
                        if ((ret = hlp_setObjectCount(value.Arr->size, &object->buffer)) != 0)
                        {
                            var_clear(&value);
                            break;
                        }
                    }
#endif //DLMS_ITALIAN_STANDARD
                    if ((ret = compactDataAppendArray(&value, &object->buffer, e->dataIndex)) != 0)
                    {
                        var_clear(&value);
                        break;
                    }
                }
                else
                {
                    if ((ret = compactDataAppend(1, &value, &object->buffer)) != 0)
                    {
                        var_clear(&value);
                        break;
                    }
                }
                var_clear(&value);
            }
            else if ((ret = compactDataAppend(0, &e->value, &object->buffer)) != 0)
            {
                break;
            }
            ve_clear(e);
        }
    }
    svr_postGet(settings, &args);
    if (ret != 0)
    {
        bb_clear(&object->buffer);
    }
    vec_empty(&args);
    return ret;
#endif //DLMS_IGNORE_MALLOC
}

int invoke_CompactData(
    dlmsServerSettings* settings,
    gxCompactData* object,
    unsigned char index)
{
    int ret;
    //Reset.
    if (index == 1)
    {
        ret = bb_clear(&object->buffer);
    }
    //Capture.
    else if (index == 2)
    {
        // Capture.
        if (object->captureMethod == DLMS_CAPTURE_METHOD_INVOKE)
        {
            ret = cosem_captureCompactData(&settings->base, object);
        }
        else
        {
            ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
    }
    return ret;
}
#endif //DLMS_IGNORE_COMPACT_DATA

#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
int invoke_DisconnectControl(
    dlmsServerSettings* settings,
    gxDisconnectControl* object,
    unsigned char index)
{
    int ret = 0;
    //Reset.
    if (index == 1)
    {
        object->controlState = DLMS_CONTROL_STATE_DISCONNECTED;
        object->outputState = 0;
    }
    //Capture.
    else if (index == 2)
    {
        object->controlState = DLMS_CONTROL_STATE_CONNECTED;
        object->outputState = 1;
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_DISCONNECT_CONTROL

#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
int invoke_LlcSscsSetup(
    gxLlcSscsSetup* object,
    unsigned char index)
{
    int ret = 0;
    //Reset.
    if (index == 1)
    {
        object->serviceNodeAddress = 0xFFE;
        object->baseNodeAddress = 0;
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_LLC_SSCS_SETUP

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
int invoke_PrimeNbOfdmPlcPhysicalLayerCounters(
    gxPrimeNbOfdmPlcPhysicalLayerCounters* object,
    unsigned char index)
{
    int ret = 0;
    //Reset.
    if (index == 1)
    {
        object->crcIncorrectCount = object->crcFailedCount = object->txDropCount = object->rxDropCount = 0;
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
int invoke_PrimeNbOfdmPlcMacCounters(
    gxPrimeNbOfdmPlcMacCounters* object,
    unsigned char index)
{
    int ret = 0;
    //Reset.
    if (index == 1)
    {
        object->txDataPktCount = object->rxDataPktCount = object->txCtrlPktCount = object->rxCtrlPktCount = object->csmaFailCount = object->csmaChBusyCount = 0;
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
int invoke_PrimeNbOfdmPlcMacNetworkAdministrationData(
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object,
    unsigned char index)
{
    int ret = 0;
    //Reset.
    if (index == 1)
    {
        arr_clear(&object->multicastEntries);
        arr_empty(&object->switchTable);
        arr_clear(&object->directTable);
        obj_clearAvailableSwitches(&object->availableSwitches);
        arr_clear(&object->communications);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA

#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
int invoke_SpecialDaysTable(
    gxSpecialDaysTable* object,
    gxValueEventArg* e)
{
    int ret = 0;
    uint16_t pos, index;
    gxSpecialDay* specialDay;
    //Insert.
    if (e->index == 1)
    {
        unsigned char append = 1;
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_checkStructure(e->parameters.byteArr, 3)) == 0 &&
            (ret = cosem_getUInt16(e->parameters.byteArr, &index)) == 0)
        {
            for (pos = 0; pos != object->entries.size; ++pos)
            {
                if ((ret = arr_getByIndex(&object->entries, object->entries.size - 1, (void**)&specialDay, sizeof(gxSpecialDay))) != 0)
                {
                    break;
                }
                //Overwrite existing item.
                if (specialDay->index == index)
                {
                    if ((ret = cosem_getDateFromOctetString(e->parameters.byteArr, &specialDay->date)) != 0 ||
                        (ret = cosem_getUInt8(e->parameters.byteArr, &specialDay->dayId)) != 0)
                    {
                    }
                    append = 0;
                    break;
                }
            }
            if (ret == 0 && append)
            {
                ++object->entries.size;
                if ((ret = arr_getByIndex(&object->entries, object->entries.size - 1, (void**)&specialDay, sizeof(gxSpecialDay))) != 0)
                {
                    --object->entries.size;
                }
                else
                {
                    specialDay->index = index;
                    if ((ret = cosem_getDateFromOctetString(e->parameters.byteArr, &specialDay->date)) != 0 ||
                        (ret = cosem_getUInt8(e->parameters.byteArr, &specialDay->dayId)) != 0)
                    {
                    }
                }
            }
        }
#else
        dlmsVARIANT* tmp3;
        dlmsVARIANT tmp2;
        if (e->parameters.Arr != NULL)
        {
            ret = va_getByIndex(e->parameters.Arr, 0, &tmp3);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            index = (uint16_t)var_toInteger(tmp3);
            for (pos = 0; pos != object->entries.size; ++pos)
            {
                if ((ret = arr_getByIndex(&object->entries, object->entries.size - 1, (void**)&specialDay)) != 0)
                {
                    break;
                }
                //Overwrite existing item.
                if (specialDay->index == index)
                {
                    append = 0;
                    break;
                }
            }
            if (append)
            {
                specialDay = (gxSpecialDay*)gxmalloc(sizeof(gxSpecialDay));
                if (specialDay == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                arr_push(&object->entries, specialDay);
            }
            specialDay->index = index;
            ret = va_getByIndex(e->parameters.Arr, 1, &tmp3);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            var_init(&tmp2);
            ret = dlms_changeType2(tmp3, DLMS_DATA_TYPE_DATE, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            time_copy(&specialDay->date, tmp2.dateTime);
            var_clear(&tmp2);
            ret = va_getByIndex(e->parameters.Arr, 2, &tmp3);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            specialDay->dayId = (unsigned char)var_toInteger(tmp3);
        }
#endif //DLMS_IGNORE_MALLOC
    }
    //Remove.
    else if (e->index == 2)
    {
        uint16_t pos, index = e->parameters.uiVal;
        for (pos = 0; pos != object->entries.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->entries, pos, (void**)&specialDay, sizeof(gxSpecialDay))) != 0)
#else
            if ((ret = arr_getByIndex(&object->entries, pos, (void**)&specialDay)) != 0)
#endif //DLMS_IGNORE_MALLOC
            {
                break;
            }
            if (specialDay->index == index)
            {
                //Remove item.
#ifdef DLMS_IGNORE_MALLOC
                ret = arr_removeByIndex(&object->entries, pos, sizeof(gxSpecialDay));
#else
                ret = arr_removeByIndex(&object->entries, pos, (void**)&specialDay);
                if (ret == 0)
                {
                    gxfree(specialDay);
                }
#endif //DLMS_IGNORE_MALLOC
                break;
            }
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE

#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
int invoke_RegisterActivation(
    dlmsServerSettings* settings,
    gxRegisterActivation* object,
    gxValueEventArg* e)
{
    int ret = 0;
    uint16_t count;
    gxRegisterActivationMask* k;
    uint16_t pos;
#ifdef DLMS_IGNORE_OBJECT_POINTERS
    gxObjectDefinition* it;
#else
    gxObject* it;
#endif //DLMS_IGNORE_OBJECT_POINTERS
    //Add Register.
    if (e->index == 1)
    {
#ifdef DLMS_IGNORE_MALLOC
        uint16_t ot;
        unsigned char ln[6];
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC))
        uint16_t count = oa_getCapacity(&object->registerAssignment);
#else
        count = arr_getCapacity(&object->registerAssignment);
        if (!(object->registerAssignment.size < count))
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
#endif //!(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC))
        if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = cosem_getUInt16(e->parameters.byteArr, &ot)) != 0 ||
            (ret = cosem_getOctetString2(e->parameters.byteArr, ln, 6, NULL)) != 0)
        {
            return ret;
        }
        if ((ret = oa_findByLN(&settings->base.objects, (DLMS_OBJECT_TYPE)ot, ln, &it)) != 0)
        {
            return ret;
        }
        if (it == NULL)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        if ((ret = arr_setByIndexRef(&object->registerAssignment, (void**)it)) != 0)
        {
            return ret;
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
#else
        dlmsVARIANT* tmp, * tmp3;
        if (e->parameters.Arr != NULL)
        {
            short type;
            ret = va_getByIndex(e->parameters.Arr, 0, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            ret = va_getByIndex(tmp->Arr, 0, &tmp3);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            type = var_toInteger(tmp3);
            ret = va_getByIndex(tmp->Arr, 1, &tmp3);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            const unsigned char* ln = tmp3->byteArr->data;
#ifdef DLMS_IGNORE_OBJECT_POINTERS
            it = (gxObjectDefinition*)gxmalloc(sizeof(gxObjectDefinition));
            it->objectType = (DLMS_OBJECT_TYPE)type;
            memcpy(it->logicalName, ln, 6);
#else
            if ((ret = oa_findByLN(&settings->base.objects, type, ln, &it)) != 0)
            {
                return ret;
            }
            if (it == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
#endif //DLMS_IGNORE_OBJECT_POINTERS
#if !defined(DLMS_IGNORE_OBJECT_POINTERS) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
            oa_push(&object->registerAssignment, it);
#else
            arr_push(&object->registerAssignment, it);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
#ifdef DLMS_IGNORE_OBJECT_POINTERS
        if (ret != 0 && objectDefinition != NULL)
        {
            gxfree(objectDefinition);
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 2)
    {
        count = arr_getCapacity(&object->maskList);
        if (!(object->maskList.size < count))
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        ++object->maskList.size;
#ifdef DLMS_IGNORE_MALLOC
        uint16_t size;
        if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) == 0 &&
            (ret = arr_getByIndex(&object->maskList, object->maskList.size - 1, (void**)&k, sizeof(gxRegisterActivationMask))) == 0 &&
            (ret = cosem_getOctetString2(e->parameters.byteArr, k->name, sizeof(k->name), &size)) == 0)
        {
            k->length = (unsigned char)size;
            size = sizeof(k->indexes);
            if ((ret = cosem_checkArray(e->parameters.byteArr, &size)) == 0)
            {
                k->count = (unsigned char)size;
                for (pos = 0; pos != k->count; ++pos)
                {
                    if ((ret = cosem_getUInt8(e->parameters.byteArr, &k->indexes[pos])) != 0)
                    {
                        break;
                    }
                }
            }
        }
#else
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    //Remove mask.
    else if (e->index == 3)
    {
        unsigned char ch;
        if ((ret = bb_getUInt8(e->parameters.byteArr, &ch)) == 0)
        {
            if (ch != DLMS_DATA_TYPE_OCTET_STRING)
            {
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            else
            {
                uint16_t count;
                if ((ret = hlp_getObjectCount2(e->parameters.byteArr, &count)) == 0)
                {
                    for (pos = 0; pos != object->maskList.size; ++pos)
                    {
                        if ((ret = arr_getByIndex2(&object->maskList, pos, (void**)&k, sizeof(gxRegisterActivationMask))) != 0)
                        {
                            break;
                        }
#if defined(DLMS_IGNORE_MALLOC)
                        if (bb_available(e->parameters.byteArr) == k->length &&
                            memcmp(k->name, e->parameters.byteArr->data + e->parameters.byteArr->position, count) == 0)
                        {
                            //arr_removeByIndex is decreasing amount already.
                            ret = arr_removeByIndex(&object->maskList, pos, sizeof(gxRegisterActivationMask));
                            break;
                        }
#else
                        if (bb_available(e->parameters.byteArr) == k->name.size &&
                            memcmp(k->name.data, e->parameters.byteArr->data + e->parameters.byteArr->position, count) == 0)
                        {
                            //arr_removeByIndex is decreasing amount already.
                            if ((ret = arr_removeByIndex(&object->maskList, pos, (void**)&k)) == 0)
                            {
                                bb_clear(&k->name);
                                bb_clear(&k->indexes);
                                gxfree(k);
                            }
                            break;
                        }
#endif //defined(DLMS_IGNORE_MALLOC)
                        e->parameters.byteArr->position += count;
                    }
                }
            }
        }
        bb_clear(e->parameters.byteArr);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER_ACTIVATION


#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR

int invoke_copySeasonProfile(gxArray* target, gxArray* source)
{
    int ret = obj_clearSeasonProfile(target);
    if (ret == 0)
    {
        int pos;
        gxSeasonProfile* sp, * it;
#if defined(DLMS_IGNORE_MALLOC)
        target->size = source->size;
#endif //
        for (pos = 0; pos != source->size; ++pos)
        {
            if (ret == 0)
            {
                if ((ret = arr_getByIndex2(source, pos, (void**)&sp, sizeof(gxSeasonProfile))) != 0)
                {
                    break;
                }
#if defined(DLMS_IGNORE_MALLOC)
                if ((ret = arr_getByIndex2(target, pos, (void**)&it, sizeof(gxSeasonProfile))) != 0)
                {
                    break;
                }
                memcpy(&it->name, &sp->name, sizeof(gxSeasonProfileName));
                it->start = sp->start;
                memcpy(&it->weekName, &sp->weekName, sizeof(gxSeasonProfileWeekName));
#else
                it = gxmalloc(sizeof(gxSeasonProfile));
                arr_push(target, it);
                BYTE_BUFFER_INIT(&it->name);
                BYTE_BUFFER_INIT(&it->weekName);
                bb_set(&it->name, sp->name.data, sp->name.size);
                it->start = sp->start;
                bb_set(&it->weekName, sp->weekName.data, sp->weekName.size);
#endif //#if defined(DLMS_IGNORE_MALLOC)
            }
        }
    }
    return ret;
}

int invoke_copyWeekProfileTable(gxArray* target, gxArray* source)
{
    int ret = obj_clearWeekProfileTable(target);
    if (ret == 0)
    {
        int pos;
        gxWeekProfile* wp, * it;
#if defined(DLMS_IGNORE_MALLOC)
        target->size = source->size;
#endif //
        for (pos = 0; pos != source->size; ++pos)
        {
            if (ret == 0)
            {
                if ((ret = arr_getByIndex2(source, pos, (void**)&wp, sizeof(gxWeekProfile))) != 0)
                {
                    break;
                }
#if defined(DLMS_IGNORE_MALLOC)
                if ((ret = arr_getByIndex2(target, pos, (void**)&it, sizeof(gxWeekProfile))) != 0)
                {
                    break;
                }
                memcpy(&it->name, &wp->name, sizeof(gxWeekProfileName));
#else
                it = gxmalloc(sizeof(gxWeekProfile));
                arr_push(target, it);
                BYTE_BUFFER_INIT(&it->name);
                bb_set(&it->name, wp->name.data, wp->name.size);
#endif //#if defined(DLMS_IGNORE_MALLOC)
                it->monday = wp->monday;
                it->tuesday = wp->tuesday;
                it->wednesday = wp->wednesday;
                it->thursday = wp->thursday;
                it->friday = wp->friday;
                it->saturday = wp->saturday;
                it->sunday = wp->sunday;
            }
        }
    }
    return ret;
}

int invoke_copyDayProfileTable(gxArray* target, gxArray* source)
{
    int ret = obj_clearDayProfileTable(target);
    if (ret == 0)
    {
        int pos, pos2;
        gxDayProfile* it, * it2;
        gxDayProfileAction* dp, * dp2;
#if defined(DLMS_IGNORE_MALLOC)
        target->size = source->size;
#endif //
        for (pos = 0; pos != source->size; ++pos)
        {
            if (ret == 0)
            {
                if ((ret = arr_getByIndex2(source, pos, (void**)&it, sizeof(gxDayProfile))) != 0)
                {
                    break;
                }
#if defined(DLMS_IGNORE_MALLOC)
                if ((ret = arr_getByIndex2(target, pos, (void**)&it2, sizeof(gxDayProfile))) != 0)
                {
                    break;
                }
                it2->daySchedules.size = it->daySchedules.size;
#else
                it2 = gxmalloc(sizeof(gxDayProfile));
                arr_push(target, it);
#endif //#if defined(DLMS_IGNORE_MALLOC)
                it2->dayId = it->dayId;
                for (pos2 = 0; pos2 != it->daySchedules.size; ++pos2)
                {
                    if ((ret = arr_getByIndex2(&it->daySchedules, pos2, (void**)&dp, sizeof(gxDayProfileAction))) != DLMS_ERROR_CODE_OK)
                    {
                        break;
                    }
#if defined(DLMS_IGNORE_MALLOC)
                    if ((ret = arr_getByIndex2(&it2->daySchedules, pos2, (void**)&dp2, sizeof(gxDayProfileAction))) != DLMS_ERROR_CODE_OK)
                    {
                        break;
                    }
#else
                    dp2 = gxmalloc(sizeof(gxDayProfileAction));
                    arr_push(&it2->daySchedules, dp2);
#endif //#if defined(DLMS_IGNORE_MALLOC)
                    dp2->startTime = dp->startTime;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    dp2->script = dp->script;
#else
                    memcpy(dp2->scriptLogicalName, dp->scriptLogicalName, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    dp2->scriptSelector = dp->scriptSelector;
                }
            }
        }
    }
    return ret;
}

int invoke_ActivityCalendar(gxValueEventArg* e)
{
    int ret;
    if (e->index == 1)
    {
        //Copy all passive items to active.
        gxActivityCalendar* object = (gxActivityCalendar*)e->target;
        bb_clear(&object->calendarNameActive);
        if ((ret = bb_set(&object->calendarNameActive, object->calendarNamePassive.data, object->calendarNamePassive.size)) != 0 ||
            (ret = invoke_copyDayProfileTable(&object->dayProfileTableActive, &object->dayProfileTablePassive)) != 0 ||
            (ret = invoke_copyWeekProfileTable(&object->weekProfileTableActive, &object->weekProfileTablePassive)) != 0 ||
            (ret = invoke_copySeasonProfile(&object->seasonProfileActive, &object->seasonProfilePassive)) != 0)
        {
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR

#ifndef DLMS_IGNORE_ARRAY_MANAGER

int invoke_ArrayManagerInsertEntry(
    dlmsServerSettings* settings,
    gxValueEventArg* e, uint16_t index)
{
    int ret;
    uint16_t count, pos, totalCount;
    uint16_t origPos = 0;
    unsigned char ch;
    dlmsVARIANT value;
    gxDataInfo info;
    di_init(&info);
    var_init(&value);
    //Move new data to the begin of the buffer.
    ret = bb_move(e->value.byteArr, e->value.byteArr->position, 0, bb_available(e->value.byteArr));
    uint16_t newDataSize = (uint16_t)e->value.byteArr->size;
    if ((ret = cosem_getValue(&settings->base, e)) == 0 && bb_size(e->value.byteArr) != 0)
    {
        if ((ret = bb_getUInt8(e->value.byteArr, &ch)) != 0 ||
            ch != DLMS_DATA_TYPE_ARRAY)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        if ((ret = hlp_getObjectCount2(e->value.byteArr, &totalCount)) != 0)
        {
            return ret;
        }
        //Change index to zero-based.
        count = (uint16_t)e->value.byteArr->size;
        e->value.byteArr->size = newDataSize + 1;
        if ((ret = hlp_setObjectCount(1 + totalCount, e->value.byteArr)) != 0)
        {
            return ret;
        }
        totalCount = 0;
        //Return original size.
        e->value.byteArr->size = count;
        for (pos = (uint16_t)e->value.byteArr->position; pos != bb_size(e->value.byteArr); ++pos)
        {
            //Get data type.
            if ((ret = bb_getUInt8ByIndex(e->value.byteArr, e->value.byteArr->position, &ch)) != 0)
            {
                break;
            }
            if (ch == DLMS_DATA_TYPE_ARRAY || ch == DLMS_DATA_TYPE_STRUCTURE)
            {
                if (index == 0)
                {
                    //Save start position.
                    origPos = (uint16_t)e->value.byteArr->position;
                }
                ++e->value.byteArr->position;
                if ((ret = hlp_getObjectCount2(e->value.byteArr, &count)) != 0)
                {
                    break;
                }
                totalCount += 1 + count;
            }
            else
            {
                if ((ret = dlms_getData(e->value.byteArr, &info, &value)) != 0)
                {
                    var_clear(&value);
                    break;
                }
#ifdef DLMS_IGNORE_MALLOC
                if (value.vt == DLMS_DATA_TYPE_OCTET_STRING)
                {
                    ++e->value.byteArr->position;
                    if ((ret = hlp_getObjectCount2(e->value.byteArr, &count)) != 0)
                    {
                        var_clear(&value);
                        break;
                    }
                    if (e->value.byteArr->size < e->value.byteArr->position + count)
                    {
                        ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                        var_clear(&value);
                        break;
                    }
                    e->value.byteArr->position += count;
                }
#endif //DLMS_IGNORE_MALLOC
                di_init(&info);
                var_clear(&value);
            }
            --totalCount;
            if (index == 0 && totalCount == 0)
            {
                if (bb_available(e->value.byteArr) == 0)
                {
                    //Move first item to last.
                    ret = bb_move(e->value.byteArr, 0, e->value.byteArr->size, newDataSize);
                    //Move all items to begin.
                    ret = bb_move(e->value.byteArr, newDataSize, 0, e->value.byteArr->size - newDataSize);
                    e->value.byteArr->position = 0;
                    ret = cosem_setValue(&settings->base, e);
                    bb_clear(e->value.byteArr);
                    break;
                }
                else if (index == 0)
                {
                    //Make space for the new item.                   
                    ret = bb_move(e->value.byteArr, origPos,
                        origPos + newDataSize, e->value.byteArr->size - newDataSize);
                    //Move new item to new location.
                    totalCount = (uint16_t)e->value.byteArr->size;
                    ret = bb_move(e->value.byteArr, 0, origPos, newDataSize);
                    e->value.byteArr->size = totalCount;
                    //Move all items to first.
                    ret = bb_move(e->value.byteArr, newDataSize, 0, e->value.byteArr->size - newDataSize);
                    e->value.byteArr->position = 0;
#ifdef DLMS_IGNORE_MALLOC
                    ret = cosem_setValue(&settings->base, e);
                    bb_clear(e->value.byteArr);
#else
                    gxByteBuffer* bb = e->value.byteArr;
                    e->value.vt = DLMS_DATA_TYPE_NONE;
                    if ((ret = dlms_getData(e->value.byteArr, &info, &e->value)))
                    {
                        bb_clear(bb);
                        var_clear(&value);
                        break;
                    }
                    bb_clear(bb);
                    if ((ret = cosem_setValue(&settings->base, e)) != 0)
                    {
                        var_clear(&value);
                        break;
                    }
#endif //DLMS_IGNORE_MALLOC
                    break;
                }
                --index;
            }
        }
    }
    return ret;
}

int invoke_ArrayManagerDeleteEntry(
    dlmsServerSettings* settings,
    gxValueEventArg* e,
    uint16_t from, uint16_t to)
{
    int ret;
    uint16_t count, pos, totalCount;
    uint16_t origPos = 0;
    unsigned char ch;
    dlmsVARIANT value;
    gxDataInfo info;
    di_init(&info);
    var_init(&value);
    if ((ret = cosem_getValue(&settings->base, e)) == 0 && bb_size(e->value.byteArr) != 0)
    {
        if ((ret = bb_getUInt8(e->value.byteArr, &ch)) != 0 ||
            ch != DLMS_DATA_TYPE_ARRAY)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        if ((ret = hlp_getObjectCount2(e->value.byteArr, &totalCount)) != 0)
        {
            return ret;
        }
        //Change from to zero-based.
        --from;
        if (to - from < totalCount)
        {
            totalCount -= to - from;
        }
        else
        {
            totalCount = from;
            to = from + 1;
        }
        count = (uint16_t)e->value.byteArr->size;
        e->value.byteArr->size = 1;
        if ((ret = hlp_setObjectCount(totalCount, e->value.byteArr)) != 0)
        {
            return ret;
        }
        totalCount = 0;
        //Return original size.
        e->value.byteArr->size = count;
        for (pos = (uint16_t)e->value.byteArr->position; pos != bb_size(e->value.byteArr); ++pos)
        {
            //Get data type.
            if ((ret = bb_getUInt8ByIndex(e->value.byteArr, e->value.byteArr->position, &ch)) != 0)
            {
                break;
            }
            if (ch == DLMS_DATA_TYPE_ARRAY || ch == DLMS_DATA_TYPE_STRUCTURE)
            {
                if (from == 0)
                {
                    //Save start position.
                    origPos = (uint16_t)e->value.byteArr->position;
                }
                ++e->value.byteArr->position;
                if ((ret = hlp_getObjectCount2(e->value.byteArr, &count)) != 0)
                {
                    break;
                }
                totalCount += 1 + count;
            }
            else
            {
                if ((ret = dlms_getData(e->value.byteArr, &info, &value)) != 0)
                {
                    var_clear(&value);
                    break;
                }
#ifdef DLMS_IGNORE_MALLOC
                if (value.vt == DLMS_DATA_TYPE_OCTET_STRING)
                {
                    ++e->value.byteArr->position;
                    if ((ret = hlp_getObjectCount2(e->value.byteArr, &count)) != 0)
                    {
                        break;
                    }
                    if (e->value.byteArr->size < e->value.byteArr->position + count)
                    {
                        ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                        break;
                    }
                    e->value.byteArr->position += count;
                }
#endif //DLMS_IGNORE_MALLOC
                di_init(&info);
                var_clear(&value);
            }
            --totalCount;
            if (totalCount == 0)
            {
                --to;
                if (to == 0)
                {
                    ret = bb_move(e->value.byteArr, e->value.byteArr->position, origPos,
                        bb_available(e->value.byteArr));
                    e->value.byteArr->position = 0;
                    ret = cosem_setValue(&settings->base, e);
                    bb_clear(e->value.byteArr);
                    break;
                }
                --from;
            }
        }
    }
    return ret;
}

int invoke_ArrayManager(
    dlmsServerSettings* settings,
    gxValueEventArg* e)
{
    int pos, ret = 0;
    gxArrayManagerItem* it;
    gxArrayManager* object = (gxArrayManager*)e->target;
    unsigned char ch, found = 0;
    uint16_t count, totalCount;
    dlmsVARIANT tmp;
    dlmsVARIANT value;
    gxDataInfo info;
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* tmp2, * tmp3;
    e->value.byteArr = &settings->info.data;
#endif//DLMS_IGNORE_MALLOC
    e->byteArray = 1;
    unsigned char id;
    uint16_t index = 0, from = 0, to = 0, origSize, origPos = 0;
    if (e->index == 1)
    {
        id = e->parameters.bVal;
        bb_clear(e->value.byteArr);
    }
    else if (e->index == 3 || e->index == 4)
    {
        //Insert or update entry.
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_getStructure(e->parameters.byteArr, &count)) != 0 ||
            (ret = cosem_getUInt8(e->parameters.byteArr, &id)) != 0 ||
            (ret = cosem_getStructure(e->parameters.byteArr, &count)) != 0 ||
            (ret = cosem_getUInt16(e->parameters.byteArr, &index)) != 0)
        {
            return ret;
        }
#else
        if ((ret = va_getByIndex(e->parameters.Arr, 0, &tmp2)) != 0)
        {
            return ret;
        }
        id = tmp2->bVal;
        if ((ret = va_getByIndex(e->parameters.Arr, 1, &tmp3)) != 0)
        {
            return ret;
        }
        if ((ret = va_getByIndex(tmp3->Arr, 0, &tmp2)) != 0)
        {
            return ret;
        }
        from = tmp2->uiVal;
        if ((ret = va_getByIndex(tmp3->Arr, 1, &tmp2)) != 0)
        {
            return ret;
        }
        to = tmp2->uiVal;
#endif //DLMS_IGNORE_MALLOC
    }
    else
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_getStructure(e->parameters.byteArr, &count)) != 0 ||
            (ret = cosem_getUInt8(e->parameters.byteArr, &id)) != 0 ||
            (ret = cosem_getStructure(e->parameters.byteArr, &count)) != 0 ||
            (ret = cosem_getUInt16(e->parameters.byteArr, &from)) != 0 ||
            (ret = cosem_getUInt16(e->parameters.byteArr, &to)) != 0)
        {
            return ret;
        }
#else
        if ((ret = va_getByIndex(e->parameters.Arr, 0, &tmp2)) != 0)
        {
            return ret;
        }
        id = tmp2->bVal;
        if ((ret = va_getByIndex(e->parameters.Arr, 1, &tmp3)) != 0)
        {
            return ret;
        }
        if ((ret = va_getByIndex(tmp3->Arr, 0, &tmp2)) != 0)
        {
            return ret;
        }
        from = tmp2->uiVal;
        if ((ret = va_getByIndex(tmp3->Arr, 1, &tmp2)) != 0)
        {
            return ret;
        }
        to = tmp2->uiVal;
#endif //DLMS_IGNORE_MALLOC
        bb_clear(e->value.byteArr);
    }
    for (pos = 0; pos != object->elements.size; ++pos)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = arr_getByIndex(&object->elements, pos, (void**)&it, sizeof(gxArrayManagerItem));
#else
        ret = arr_getByIndex(&object->elements, pos, (void**)&it);
#endif //DLMS_IGNORE_MALLOC
        if (ret != 0)
        {
            break;
        }
        if (it->id == id)
        {
            if (e->index == 1)
            {
                //Number Of Entries.
                if (it->element.target->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC
                    && it->element.attributeIndex == 2)
                {
                    //Entries in use is returned when buffer size is asked.
                    GX_UINT32(tmp) = ((gxProfileGeneric*)it->element.target)->entriesInUse;
                }
                else
                {
                    e->index = it->element.attributeIndex;
                    e->target = it->element.target;
                    if ((ret = cosem_getValue(&settings->base, e)) != 0 ||
                        (ret = bb_getUInt8(e->value.byteArr, &ch)) != 0)
                    {
                        break;
                    }
                    if (ch != DLMS_DATA_TYPE_ARRAY)
                    {
                        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
                        break;
                    }
                    if (hlp_getObjectCount2(e->value.byteArr, &count) != 0)
                    {
                        break;
                    }
                    if (count < 0x100)
                    {
                        GX_UINT8(tmp) = (unsigned char)count;
                    }
                    else
                    {
                        GX_UINT16(tmp) = count;
                    }
                }
                bb_clear(e->value.byteArr);
                if ((ret = dlms_setData(e->value.byteArr, tmp.vt, &tmp)) != 0)
                {
                    break;
                }
                found = 1;
                break;
            }
            else if (e->index == 2)
            {
                //Retrieve entries.
                if (it->element.target->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
                {

                }
                if (!found)
                {
                    di_init(&info);
                    var_init(&value);
                    e->index = it->element.attributeIndex;
                    e->target = it->element.target;
                    if ((ret = cosem_getValue(&settings->base, e)) == 0 && bb_size(e->value.byteArr) != 0)
                    {
                        if ((ret = bb_getUInt8(e->value.byteArr, &ch)) != 0 ||
                            ch != DLMS_DATA_TYPE_ARRAY)
                        {
                            break;
                        }
                        if ((ret = hlp_getObjectCount2(e->value.byteArr, &totalCount)) != 0)
                        {
                            break;
                        }
                        //Change from to zero-based.
                        --from;
                        if (to - from < totalCount)
                        {
                            totalCount = to - from;
                        }
                        else if (totalCount < to)
                        {
                            if (totalCount == 0)
                            {
                                to = 1;
                            }
                            else
                            {
                                to = totalCount;
                            }
                        }
                        count = (uint16_t)e->value.byteArr->size;
                        e->value.byteArr->size = 1;
                        if ((ret = hlp_setObjectCount(totalCount, e->value.byteArr)) != 0)
                        {
                            break;
                        }
                        totalCount = 0;
                        origSize = (uint16_t)e->value.byteArr->size;
                        //Return original size.
                        e->value.byteArr->size = count;
                        for (pos = e->value.byteArr->position; pos != bb_size(e->value.byteArr); ++pos)
                        {
                            //Get data type.
                            if ((ret = bb_getUInt8ByIndex(e->value.byteArr, e->value.byteArr->position, &ch)) != 0)
                            {
                                break;
                            }
                            if (ch == DLMS_DATA_TYPE_ARRAY || ch == DLMS_DATA_TYPE_STRUCTURE)
                            {
                                if (from == 0)
                                {
                                    //Save start position.
                                    origPos = (uint16_t)e->value.byteArr->position;
                                }
                                ++e->value.byteArr->position;
                                if ((ret = hlp_getObjectCount2(e->value.byteArr, &count)) != 0)
                                {
                                    break;
                                }
                                totalCount += 1 + count;
                            }
                            else
                            {
                                if ((ret = dlms_getData(e->value.byteArr, &info, &value)) != 0)
                                {
                                    var_clear(&value);
                                    break;
                                }
#ifdef DLMS_IGNORE_MALLOC
                                if (value.vt == DLMS_DATA_TYPE_OCTET_STRING)
                                {
                                    ++e->value.byteArr->position;
                                    if ((ret = hlp_getObjectCount2(e->value.byteArr, &count)) != 0)
                                    {
                                        break;
                                    }
                                    if (e->value.byteArr->size < e->value.byteArr->position + count)
                                    {
                                        ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                                        break;
                                    }
                                    e->value.byteArr->position += count;
                                }
#endif //DLMS_IGNORE_MALLOC

                                di_init(&info);
                                var_clear(&value);
                            }
                            --totalCount;
                            if (totalCount == 0)
                            {
                                --to;
                                if (to == 0)
                                {
                                    ret = bb_move(e->value.byteArr, origPos,
                                        origSize, e->value.byteArr->position - origPos);
                                    e->value.byteArr->position = 0;
                                    break;
                                }
                                --from;
                            }
                        }
                        found = 1;
                        break;
                    }
                }
                break;
            }
            else if (e->index == 3)
            {
                e->index = it->element.attributeIndex;
                e->target = it->element.target;
                ret = invoke_ArrayManagerInsertEntry(settings, e, index);
                found = 1;
                break;
            }
            else if (e->index == 4)
            {
                e->index = it->element.attributeIndex;
                e->target = it->element.target;
                ret = invoke_ArrayManagerDeleteEntry(settings, e, index, index);
                ret = invoke_ArrayManagerInsertEntry(settings, e, index);
                found = 1;
                break;
            }
            else if (e->index == 5)
            {
                e->index = it->element.attributeIndex;
                e->target = it->element.target;
                ret = invoke_ArrayManagerDeleteEntry(settings, e, from, to);
                found = 1;
                break;
            }
        }
    }
    if (!found)
    {
        bb_clear(e->value.byteArr);
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
    }
    return ret;
}
#endif //DLMS_IGNORE_ARRAY_MANAGER

#ifndef DLMS_IGNORE_G3_PLC_MAC_SETUP
int invoke_G3_PLC_MAC_Setup(
    dlmsServerSettings* settings,
    gxG3PlcMacSetup* object,
    gxValueEventArg* e)
{
    uint16_t address, pos, count = 0;
    int ret = 0;
    e->byteArray = 1;
    bb_clear(&settings->info.data);
    e->value.byteArr = &settings->info.data;
    if (e->parameters.vt != DLMS_DATA_TYPE_UINT16)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    address = e->parameters.uiVal;
    if (e->index == 1)
    {
        //Get amount of neighbour tables. 
        gxNeighbourTable* it;
        for (pos = 0; pos < (uint16_t)object->neighbourTable.size; ++pos)
        {
#ifndef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->neighbourTable, pos, (void**)&it)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(&object->neighbourTable, pos,
                (void**)&it, sizeof(gxNeighbourTable))) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC        
            if (it->shortAddress == address)
            {
                ++count;
            }
        }
        if (ret == 0)
        {
            return cosem_getG3PlcMacSetupNeighbourTables(&object->neighbourTable, address, count, e);
        }
    }
    else if (e->index == 2)
    {
        gxMacPosTable* it;
        //Get amount of MAC POS tables. 
        for (pos = 0; pos < object->macPosTable.size; ++pos)
        {
#ifndef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->macPosTable, pos, (void**)&it)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(&object->macPosTable, pos, (void**)&it, sizeof(gxMacPosTable))) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if (it->shortAddress == address)
            {
                ++count;
            }
        }
        if (ret == 0)
        {
            return cosem_getG3PlcMacSetupPosTables(&object->macPosTable, address, count, e);
        }
    }
    bb_clear(e->value.byteArr);
    return DLMS_ERROR_CODE_READ_WRITE_DENIED;
}
#endif //DLMS_IGNORE_G3_PLC_MAC_SETUP

#ifndef DLMS_IGNORE_FUNCTION_CONTROL

int invoke_removeFromActivationStatusList(gxArray* arr, unsigned char* name, uint16_t len)
{
    uint16_t pos;
    functionStatus* fs;
    int ret = 0;
    for (pos = 0; pos != arr->size; ++pos)
    {
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(arr, pos, (void**)&fs)) != 0)
        {
            break;
        }
        if (fs->name.size == len && memcmp(fs->name.data, name, len) == 0)
        {
            ret = arr_removeByIndex(arr, pos, (void**)&fs);
            bb_clear(&fs->name);
            gxfree(fs);
            break;
        }
#else
        if ((ret = arr_getByIndex(arr, pos, (void**)&fs, sizeof(functionStatus))) != 0)
        {
            break;
        }
        if (fs->size == len && memcmp(fs->name, name, len) == 0)
        {
            ret = arr_removeByIndex(arr, pos, sizeof(functionStatus));
            break;
        }
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}


int invoke_removeFromFunctionList(gxArray* arr, unsigned char* name, uint16_t len)
{
    uint16_t pos;
    functionalBlock* fb;
    int ret = 0;
    for (pos = 0; pos != arr->size; ++pos)
    {
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(arr, pos, (void**)&fb)) != 0)
        {
            break;
        }
        if (fb->name.size == len && memcmp(fb->name.data, name, len) == 0)
        {
            ret = arr_removeByIndex(arr, pos, (void**)&fb);
            bb_clear(&fb->name);
            gxfree(fb);
            break;
        }
#else
        if ((ret = arr_getByIndex(arr, pos, (void**)&fb, sizeof(functionalBlock))) != 0)
        {
            break;
        }
        if (fb->nameSize == len && memcmp(fb->name, name, len) == 0)
        {
            ret = arr_removeByIndex(arr, pos, sizeof(functionalBlock));
            break;
        }
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

int invoke_FunctionControl(
    dlmsServerSettings* settings,
    gxFunctionControl* object,
    gxValueEventArg* e)
{
    uint16_t pos, pos2, ot, count = 0;
    uint8_t ch;
    int ret = 0;
    functionStatus* fs;
    functionalBlock* fb;
    unsigned char* name, * ln;
    uint16_t len, lnLen;
    if (e->index == 1)
    {
        count = arr_getCapacity(&object->activationStatus);
        if ((ret = cosem_checkArray(e->value.byteArr, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_checkStructure(e->value.byteArr, 2)) != 0)
                {
                    break;
                }
                if ((ret = bb_getUInt8(e->parameters.byteArr, &ch)) != 0)
                {
                    return ret;
                }
                if (ch != DLMS_DATA_TYPE_OCTET_STRING)
                {
                    ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                    break;
                }
                if ((ret = hlp_getObjectCount2(e->parameters.byteArr, &len)) != 0)
                {
                    return ret;
                }
                if (e->parameters.byteArr->position + len > e->parameters.byteArr->size)
                {
                    ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                    break;
                }
                name = e->parameters.byteArr->data + e->parameters.byteArr->position;
                e->parameters.byteArr->position += len;
                if ((ret = cosem_getBoolean(e->value.byteArr, &ch)) != 0)
                {
                    break;
                }
                for (pos2 = 0; pos2 != object->activationStatus.size; ++pos2)
                {
#ifndef DLMS_IGNORE_MALLOC
                    if ((ret = arr_getByIndex(&object->activationStatus, pos2, (void**)&fs)) != 0)
                    {
                        break;
                    }
                    if (fs->name.size == len && memcmp(fs->name.data, name, len) == 0)
                    {
                        fs->status = ch;
                        break;
                    }
#else
                    if ((ret = arr_getByIndex(&object->activationStatus, pos2, (void**)&fs, sizeof(functionStatus))) != 0)
                    {
                        break;
                    }
                    if (fs->size == len && memcmp(fs->name, name, len) == 0)
                    {
                        fs->status = ch;
                        break;
                    }
#endif //DLMS_IGNORE_MALLOC
                }
            }
        }
    }
    else if (e->index == 2)
    {
        if ((ret = cosem_checkStructure(e->value.byteArr, 2)) != 0 ||
            (ret = bb_getUInt8(e->parameters.byteArr, &ch)) != 0)
        {
            return ret;
        }
        if (ch != DLMS_DATA_TYPE_OCTET_STRING)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        if ((ret = hlp_getObjectCount2(e->parameters.byteArr, &len)) != 0)
        {
            return ret;
        }
        if (e->parameters.byteArr->position + len > e->parameters.byteArr->size)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        name = e->parameters.byteArr->data + e->parameters.byteArr->position;
        e->parameters.byteArr->position += len;
        if ((ret = invoke_removeFromActivationStatusList(&object->activationStatus, name, len)) == 0)
        {
            ret = invoke_removeFromFunctionList(&object->functions, name, len);
        }
#ifndef DLMS_IGNORE_MALLOC
        fb = (functionalBlock*)gxmalloc(sizeof(functionalBlock));
        if (fb == NULL)
        {
            ret = DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        fs = (functionStatus*)gxmalloc(sizeof(functionStatus));
        if (fs == NULL)
        {
            ret = DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        if (ret == 0)
        {
            BYTE_BUFFER_INIT(&fb->name);
            bb_set(&fb->name, name, len);
            oa_init(&fb->functionSpecifications);
            if ((ret = arr_push(&object->functions, fb)) != 0)
            {
                gxfree(fb);
            }
            BYTE_BUFFER_INIT(&fs->name);
            bb_set(&fs->name, name, len);
            if ((ret = arr_push(&object->activationStatus, fb)) != 0)
            {
                --object->functions.size;
                gxfree(fb);
                gxfree(fs);
            }
        }
#else
        if (ret == 0 && object->functions.size < object->functions.capacity &&
            object->activationStatus.size < object->activationStatus.capacity)
        {
            ++object->functions.size;
            if ((ret = arr_getByIndex(&object->functions, object->functions.size - 1, (void**)&fb, sizeof(functionalBlock))) != 0)
            {
                --object->functions.size;
            }
            ++object->activationStatus.size;
            if ((ret = arr_getByIndex(&object->activationStatus, object->activationStatus.size - 1, (void**)&fs, sizeof(functionStatus))) != 0)
            {
                --object->functions.size;
                --object->activationStatus.size;
            }
            if (sizeof(fb->name) < len ||
                sizeof(fs->name) < len)
            {
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            else
            {
                memcpy(fb->name, name, len);
                fb->nameSize = len;
                fb->functionSpecificationsSize = 0;
                memcpy(fs->name, name, len);
                fs->size = len;
            }
        }
        else
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
#endif //DLMS_IGNORE_MALLOC
        count = arr_getCapacity(&object->functions);
        if (ret == 0 && (ret = cosem_checkArray(e->value.byteArr, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_checkStructure(e->value.byteArr, 2)) != 0)
                {
                    break;
                }
                if ((ret = cosem_getUInt16(e->value.byteArr, &ot)) != 0)
                {
                    break;
                }
                if ((ret = bb_getUInt8(e->parameters.byteArr, &ch)) != 0)
                {
                    return ret;
                }
                if (ch != DLMS_DATA_TYPE_OCTET_STRING)
                {
                    ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                    break;
                }
                if ((ret = hlp_getObjectCount2(e->parameters.byteArr, &lnLen)) != 0)
                {
                    return ret;
                }
                if (e->parameters.byteArr->position + lnLen > e->parameters.byteArr->size)
                {
                    ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                    break;
                }
                ln = e->parameters.byteArr->data + e->parameters.byteArr->position;
                e->parameters.byteArr->position += lnLen;
                if (ret == 0)
                {
                    //Add new function
                    gxObject* obj;
                    ret = cosem_findObjectByLN(&settings->base, (DLMS_OBJECT_TYPE)ot, ln, &obj);
                    if (ret == 0)
                    {
#ifndef DLMS_IGNORE_MALLOC
                        if ((ret = oa_push(&fb->functionSpecifications, obj)) != 0)
                        {
                            break;
                        }
#else
                        if (ret == 0 && fb->functionSpecificationsSize < MAX_FUNCTION_TARGET_LENGTH)
                        {
                            fb->functionSpecifications[fb->functionSpecificationsSize] = obj;
                            ++fb->functionSpecificationsSize;
                        }
                        else
                        {
                            --fb->functionSpecificationsSize;
                            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                            break;
                        }
#endif //DLMS_IGNORE_MALLOC
                    }
                }
            }
        }
    }
    else if (e->index == 3)
    {
        //Remove item from the function list and activation status.
        if ((ret = bb_getUInt8(e->parameters.byteArr, &ch)) != 0)
        {
            return ret;
        }
        if (ch != DLMS_DATA_TYPE_OCTET_STRING)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        if ((ret = hlp_getObjectCount2(e->parameters.byteArr, &len)) != 0)
        {
            return ret;
        }
        if (e->parameters.byteArr->position + len > e->parameters.byteArr->size)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        name = e->parameters.byteArr->data + e->parameters.byteArr->position;
        if ((ret = invoke_removeFromActivationStatusList(&object->activationStatus, name, len)) == 0)
        {
            ret = invoke_removeFromFunctionList(&object->functions, name, len);
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    bb_clear(e->value.byteArr);
    return ret;
}
#endif //DLMS_IGNORE_FUNCTION_CONTROL

#ifndef DLMS_IGNORE_NTP_SETUP

int invoke_removeFromNtpKeys(gxArray* arr, uint32_t id)
{
    uint16_t pos;
#ifndef DLMS_IGNORE_MALLOC
    gxKey3* it;
#else
    gxNtpKey* it;
#endif //DLMS_IGNORE_MALLOC
    int ret = 0;
    for (pos = 0; pos != arr->size; ++pos)
    {
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(arr, pos, (void**)&it)) != 0)
        {
            break;
        }
        if (it->key == id)
        {
            ret = arr_removeByIndex(arr, pos, (void**)&it);
            bb_clear((gxByteBuffer*)it->value);
            gxfree(it);
            break;
        }
#else
        if ((ret = arr_getByIndex(arr, pos, (void**)&it, sizeof(gxNtpKey))) != 0)
        {
            break;
        }
        if (it->id == id)
        {
            ret = arr_removeByIndex(arr, pos, sizeof(gxNtpKey));
            break;
        }
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

int invoke_NtpSetup(
    dlmsServerSettings* settings,
    gxNtpSetup* object,
    gxValueEventArg* e)
{
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    gxKey3* it;
#else
    gxNtpKey* it;
#endif //DLMS_IGNORE_MALLOC
    if (e->index == 1)
    {
        //server implements Synchronize
    }
    else if (e->index == 2)
    {
        //Add authentication key.
        if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) == 0)
        {
#ifndef DLMS_IGNORE_MALLOC
            it = (gxKey3*)gxmalloc(sizeof(gxKey3));
            it->value = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
            BYTE_BUFFER_INIT(it->value);
            if ((ret = cosem_getUInt32(e->parameters.byteArr, &it->key)) != 0 ||
                (ret = cosem_getOctetString(e->parameters.byteArr, (gxByteBuffer*)it->value)) != 0 ||
                (ret = arr_push(&object->keys, it)) != 0)
            {
            }
#else
            if (arr_getCapacity(&object->keys) < object->keys.size)
            {
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            else
            {
                ++object->keys.size;
                if ((ret = arr_getByIndex(&object->keys, object->keys.size - 1, (void**)&it, sizeof(gxNtpKey))) != 0 ||
                    (ret = cosem_getUInt32(e->parameters.byteArr, &it->id)) != 0 ||
                    (ret = cosem_getOctetString2(e->parameters.byteArr, it->key, MAX_AUTHENTICATION_KEY_LENGTH, &it->size)) != 0)
                {
                    --object->keys.size;
                }
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    else if (e->index == 3)
    {
        //Delete authentication key.
        if (e->parameters.vt == DLMS_DATA_TYPE_UINT32)
        {
            ret = invoke_removeFromNtpKeys(&object->keys, e->parameters.uiVal);
        }
        else
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_NTP_SETUP

#ifndef DLMS_IGNORE_IP6_SETUP

int invoke_removeFromIps(gxArray* arr, IN6_ADDR* ip)
{
    uint16_t pos;
    IN6_ADDR* it;
    int ret = 0;
    for (pos = 0; pos != arr->size; ++pos)
    {
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(arr, pos, (void**)&it)) != 0)
        {
            break;
        }
        if (memcmp(it, ip, sizeof(IN6_ADDR)) == 0)
        {
            ret = arr_removeByIndex(arr, pos, (void**)&it);
            gxfree(it);
            break;
        }
#else
        if ((ret = arr_getByIndex(arr, pos, (void**)&it, sizeof(IN6_ADDR))) != 0)
        {
            break;
        }
        if (memcmp(it, ip, sizeof(IN6_ADDR)) == 0)
        {
            ret = arr_removeByIndex(arr, pos, sizeof(IN6_ADDR));
            break;
        }
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

int invoke_appendToIps(gxArray* arr, IN6_ADDR* ip)
{
#ifndef DLMS_IGNORE_MALLOC
    return arr_push(arr, ip);
#else
    int ret;
    IN6_ADDR* it;
    if (arr_getCapacity(arr) < arr->size)
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    else
    {
        ++arr->size;
        if ((ret = arr_getByIndex(arr, arr->size - 1, (void**)&it, sizeof(IN6_ADDR))) != 0)
        {
            --arr->size;
        }
        else
        {
            memcpy(it, ip, sizeof(IN6_ADDR));
        }
    }
    return ret;
#endif //DLMS_IGNORE_MALLOC
}

int invoke_Ip6Setup(
    dlmsServerSettings* settings,
    gxIp6Setup* object,
    gxValueEventArg* e)
{
    int ret;
    unsigned char type;
#ifndef DLMS_IGNORE_MALLOC
    IN6_ADDR* ip = (IN6_ADDR*)gxmalloc(sizeof(IN6_ADDR));
    if (e->parameters.vt == DLMS_DATA_TYPE_STRUCTURE && e->parameters.Arr->size == 2)
    {
        dlmsVARIANT* tmp;
        if ((ret = va_getByIndex(e->parameters.Arr, 0, &tmp)) == 0)
        {
            type = var_toInteger(tmp);
            if ((ret = va_getByIndex(e->parameters.Arr, 1, &tmp)) == 0)
            {
                memcpy(ip, (IN6_ADDR*)tmp->byteArr->data, sizeof(IN6_ADDR));
            }
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (ret == 0)
#else
    IN6_ADDR tmp;
    IN6_ADDR* ip = &tmp;
    if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) == 0 &&
        (ret = cosem_getEnum(e->parameters.byteArr, &type)) == 0 &&
        (ret = cosem_getOctetString2(e->parameters.byteArr, (unsigned char*)ip, sizeof(IN6_ADDR), NULL)) == 0)
#endif //DLMS_IGNORE_MALLOC
    {
        if (e->index == 1)
        {
            //Add IP address.
            switch (type)
            {
            case DLMS_IPV6_ADDRESS_TYPE_UNICAST:
                ret = invoke_appendToIps(&object->unicastIPAddress, ip);
                break;
            case DLMS_IPV6_ADDRESS_TYPE_MULTICAST:
                ret = invoke_appendToIps(&object->multicastIPAddress, ip);
                break;
            case DLMS_IPV6_ADDRESS_TYPE_GATEWAY:
                ret = invoke_appendToIps(&object->gatewayIPAddress, ip);
                break;
            default:
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                break;
            }
        }
        else if (e->index == 2)
        {
            switch (type)
            {
            case DLMS_IPV6_ADDRESS_TYPE_UNICAST:
                ret = invoke_removeFromIps(&object->unicastIPAddress, ip);
                break;
            case DLMS_IPV6_ADDRESS_TYPE_MULTICAST:
                ret = invoke_removeFromIps(&object->multicastIPAddress, ip);
                break;
            case DLMS_IPV6_ADDRESS_TYPE_GATEWAY:
                ret = invoke_removeFromIps(&object->gatewayIPAddress, ip);
                break;
            default:
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                break;
            }
        }
        else
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_IP6_SETUP

int cosem_invoke(
    dlmsServerSettings* settings,
    gxValueEventArg* e)
{
    int ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    //If invoke index is invalid.
    if (e->index < 1 || e->index > obj_methodCount(e->target))
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    switch (e->target->objectType)
    {
#ifndef DLMS_IGNORE_REGISTER
    case DLMS_OBJECT_TYPE_REGISTER:
        ret = invoke_Register(
            (gxRegister*)e->target,
            e);
        break;
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
    case DLMS_OBJECT_TYPE_CLOCK:
        ret = invoke_Clock(
            (gxClock*)e->target,
            e->index, &e->parameters);
        break;
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        ret = invoke_ExtendedRegister(settings,
            (gxExtendedRegister*)e->target,
            e->index);
        break;
#endif //DLMS_IGNORE_EXTENDED_REGISTER
#ifndef DLMS_IGNORE_DEMAND_REGISTER
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        ret = invoke_DemandRegister(settings,
            (gxDemandRegister*)e->target,
            e->index);
        break;
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_PROFILE_GENERIC
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        ret = invoke_ProfileGeneric(
            settings,
            (gxProfileGeneric*)e->target,
            e->index);
        break;
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        ret = invoke_ScriptTable(settings, e);
        break;
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
        ret = invoke_zigbeeNetworkControl(
            (gxZigBeeNetworkControl*)e->target,
            e->index,
            &e->parameters);
        break;
#endif //DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
#ifndef DLMS_IGNORE_CHARGE
    case DLMS_OBJECT_TYPE_CHARGE:
        ret = invoke_Charge(
            (gxCharge*)e->target,
            e->index,
            &e->parameters);
        break;
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_CREDIT
    case DLMS_OBJECT_TYPE_CREDIT:
        ret = invoke_Credit(
            (gxCredit*)e->target,
            e->index,
            &e->parameters);
        break;
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        ret = invoke_gxTokenGateway(
            (gxTokenGateway*)e->target,
            e->index,
            &e->parameters);
        break;
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = invoke_AssociationShortName(settings, e);
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        break;
#endif //ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        ret = invoke_AssociationLogicalName(settings, e);
        break;
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        ret = invoke_ImageTransfer((gxImageTransfer*)e->target, e);
        break;
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
        ret = invoke_SapAssigment((gxSapAssignment*)e->target, e);
        break;
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_SECURITY_SETUP
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        ret = invoke_SecuritySetup(settings, (gxSecuritySetup*)e->target, e);
        break;
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_COMPACT_DATA
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        ret = invoke_CompactData(settings, (gxCompactData*)e->target, e->index);
        break;
#endif //DLMS_IGNORE_COMPACT_DATA
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        ret = invoke_DisconnectControl(settings, (gxDisconnectControl*)e->target, e->index);
        break;
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
    case DLMS_OBJECT_TYPE_LLC_SSCS_SETUP:
        ret = invoke_LlcSscsSetup((gxLlcSscsSetup*)e->target, e->index);
        break;
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS:
        ret = invoke_PrimeNbOfdmPlcPhysicalLayerCounters((gxPrimeNbOfdmPlcPhysicalLayerCounters*)e->target, e->index);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS:
        ret = invoke_PrimeNbOfdmPlcMacCounters((gxPrimeNbOfdmPlcMacCounters*)e->target, e->index);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA:
        ret = invoke_PrimeNbOfdmPlcMacNetworkAdministrationData((gxPrimeNbOfdmPlcMacNetworkAdministrationData*)e->target, e->index);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        ret = invoke_SpecialDaysTable((gxSpecialDaysTable*)e->target, e);
        break;
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        ret = invoke_RegisterActivation(settings, (gxRegisterActivation*)e->target, e);
        break;
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        ret = invoke_ActivityCalendar(e);
        break;
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_ARRAY_MANAGER
    case DLMS_OBJECT_TYPE_ARRAY_MANAGER:
        ret = invoke_ArrayManager(settings, e);
        break;
#endif //DLMS_IGNORE_ARRAY_MANAGER
#ifndef DLMS_IGNORE_G3_PLC_MAC_SETUP
    case DLMS_OBJECT_TYPE_G3_PLC_MAC_SETUP:
        ret = invoke_G3_PLC_MAC_Setup(settings, (gxG3PlcMacSetup*)e->target, e);
        break;
#endif //DLMS_IGNORE_G3_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_FUNCTION_CONTROL
    case DLMS_OBJECT_TYPE_FUNCTION_CONTROL:
        ret = invoke_FunctionControl(settings, (gxFunctionControl*)e->target, e);
        break;
#endif //DLMS_IGNORE_FUNCTION_CONTROL
#ifndef DLMS_IGNORE_NTP_SETUP
    case DLMS_OBJECT_TYPE_NTP_SETUP:
        ret = invoke_NtpSetup(settings, (gxNtpSetup*)e->target, e);
        break;
#endif //DLMS_IGNORE_NTP_SETUP
#ifndef DLMS_IGNORE_IP6_SETUP
    case DLMS_OBJECT_TYPE_IP6_SETUP:
        ret = invoke_Ip6Setup(settings, (gxIp6Setup*)e->target, e);
        break;
#endif //DLMS_IGNORE_IP6_SETUP
    default:
        //Unknown type.
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_SERVER
