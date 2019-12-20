#include "india.h"

///////////////////////////////////////////////////////////////////////
/*All objects.*/
static gxObject* ALL_OBJECTS[] = {
    BASE(llsSecretKey),
    BASE(clock1),
    BASE(cumulativeBillingCount),
    BASE(cosemLogicalDeviceName),
    BASE(cumulativeTamperCount),
    BASE(meterType),
    BASE(meterSerialNumber),
    BASE(manufacturerName),
    BASE(meterYearOfManufacture),
    BASE(cumulativeProgrammingCount),
    BASE(noOfPowerFailuresInAllThreePhases),
    BASE(eventCode1),
    BASE(eventCurrentRelated),
    BASE(eventPowerRelated),
    BASE(eventTransactionRelated),
    BASE(eventOthers),
    BASE(eventNonRollOverEvents),
    BASE(eventLoadSwitchStatus),
    BASE(firmwareVersionForMeter),
    BASE(transformerRatioMinusCurrent),
    BASE(transformerRatioMinusVoltage),
    BASE(demandIntegrationPeriod),
    BASE(profileCapturePeriod),
    BASE(recordingInterval2ForLoadProfile),
    BASE(timeStampOfTheMostRecentBillingPeriodClosed),
    BASE(maximumDemandKw),
    BASE(sumLiActivePowerMinusMax1RateIsTotal),
    BASE(maximumDemandKva),
    BASE(sumLiApparentPowerMinusMax1RateIsTotal),
    BASE(iecHdlcSetup),
    BASE(voltageRelatedEventsProfile),
    BASE(currentRelatedEventsProfile),
    BASE(powerRelatedEventsProfile),
    BASE(transactionEventsProfile),
    BASE(otherTamperEventsProfile),
    BASE(nonRollOverEventsProfile),
    BASE(controlEventsProfile),
    BASE(instantaneousProfile),
    BASE(scalerInstantaneousProfile),
    BASE(scalerBlockLoadProfile),
    BASE(scalerDailyLoadProfile),
    BASE(scalerBillingProfile),
    BASE(scalerEventsProfile),
    BASE(billingProfile),
    BASE(loadProfile),
    BASE(dailyLoadProfile),
    BASE(billingDateImportMode),
    BASE(cumulativePowerOffDurationInMin),
    BASE(activePowerKw),
    BASE(cumulativeEnergyKwh),
    BASE(cumulativeEnergyKwhExport),
    BASE(sumLiReactivePowerPlusInstValue),
    BASE(sumLiReactivePowerQiTimeIntegral1RateIsTotal),
    BASE(sumLiReactivePowerQiiTimeIntegral1RateIsTotal),
    BASE(sumLiReactivePowerQiiiTimeIntegral1RateIsTotal),
    BASE(sumLiReactivePowerQivTimeIntegral1RateIsTotal),
    BASE(apparentPowerKva),
    BASE(cumulativeEnergyKvahImport),
    BASE(cumulativeEnergyKvahExport),
    BASE(signedPowerFactor),
    BASE(frequencyHz),
    BASE(l1CurrentInstValue),
    BASE(l1VoltageInstValue),
    BASE(l1PowerFactorInstValue),
    BASE(l2CurrentInstValue),
    BASE(l2VoltageInstValue),
    BASE(l2PowerFactorInstValue),
    BASE(l3CurrentInstValue),
    BASE(l3VoltageInstValue),
    BASE(l3PowerFactorInstValue),
    BASE(sumLiActivePowerTimeIntegral1RateIsTotal),
    BASE(l1CurrentCurrentAvg5),
    BASE(l2CurrentCurrentAvg5),
    BASE(l3CurrentCurrentAvg5),
    BASE(l1VoltageCurrentAvg5),
    BASE(l2VoltageCurrentAvg5),
    BASE(l3VoltageCurrentAvg5),
    BASE(blockEnergyKwhImport),
    BASE(blockEnergyKwhExport),
    BASE(sumLiActivePowerTimeIntegral5),
    BASE(blockEnergyKvahImport),
    BASE(blockEnergyKvahExport),
    BASE(sumLiReactivePowerQiTimeIntegral5),
    BASE(sumLiReactivePowerQiiTimeIntegral5),
    BASE(sumLiReactivePowerQiiiTimeIntegral5),
    BASE(sumLiReactivePowerQivTimeIntegral5),
    BASE(identifiersForIndia),
};

int addllsSecretKey()
{
    int ret;
    //Dedicated key.
    static unsigned char CYPHERING_INFO[20] = { 0 };
    const unsigned char ln[6] = { 0, 0, 40, 0, 2, 255 };
    if ((ret = INIT_OBJECT(llsSecretKey, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        OA_ATTACH(llsSecretKey.objectList, ALL_OBJECTS);
        llsSecretKey.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_NONE;
        BB_ATTACH(llsSecretKey.xDLMSContextInfo.cypheringInfo, CYPHERING_INFO, 0);
        llsSecretKey.clientSAP = 32;
        llsSecretKey.serverSAP = 1;
    }
    return ret;
}

int addclock1()
{
    int ret;
    //Add default clock. clock1's Logical Name is 0.0.1.0.0.255.
    const unsigned char ln[6] = { 0, 0, 1, 0, 0, 255 };
    if ((ret = INIT_OBJECT(clock1, DLMS_OBJECT_TYPE_CLOCK, ln)) == 0)
    {
        time_init(&clock1.begin, 2017, 4, 1, 4, 20, 9, 255, 180);
        time_init(&clock1.end, 2017, 4, 1, 4, 20, 10, 255, 180);
        //Meter time zone.
        clock1.timeZone = 330;
        //Meter Deviation in minutes.
        clock1.deviation = 0;
    }
    return ret;
}

int addcumulativeBillingCount()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 0, 1, 0, 255 };
    if ((ret = INIT_OBJECT(cumulativeBillingCount, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT32(cumulativeBillingCount.value) = 1;
    }
    return ret;
}

int addcosemLogicalDeviceName()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 42, 0, 0, 255 };
    if ((ret = INIT_OBJECT(cosemLogicalDeviceName, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        static unsigned char data[16];


        GX_OCTECT_STRING(cosemLogicalDeviceName.value, data, 16);
    }
    return ret;
}

int addcumulativeTamperCount()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 94, 91, 0, 255 };
    if ((ret = INIT_OBJECT(cumulativeTamperCount, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(cumulativeTamperCount.value) = 0;
    }
    return ret;
}

int addmeterType()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 94, 91, 9, 255 };
    if ((ret = INIT_OBJECT(meterType, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT8(meterType.value) = 1;
    }
    return ret;
}

int addmeterSerialNumber()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 96, 1, 0, 255 };
    if ((ret = INIT_OBJECT(meterSerialNumber, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT32(meterSerialNumber.value) = 664183;
    }
    return ret;
}

int addmanufacturerName()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 96, 1, 1, 255 };
    if ((ret = INIT_OBJECT(manufacturerName, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        static char data[301];


        GX_OCTECT_STRING(manufacturerName.value, data, 30);
    }
    return ret;
}

int addmeterYearOfManufacture()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 96, 1, 4, 255 };
    if ((ret = INIT_OBJECT(meterYearOfManufacture, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(meterYearOfManufacture.value) = 2017;
    }
    return ret;
}

int addcumulativeProgrammingCount()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 96, 2, 0, 255 };
    if ((ret = INIT_OBJECT(cumulativeProgrammingCount, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(cumulativeProgrammingCount.value) = 0;
    }
    return ret;
}

int addnoOfPowerFailuresInAllThreePhases()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 96, 7, 0, 255 };
    if ((ret = INIT_OBJECT(noOfPowerFailuresInAllThreePhases, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(noOfPowerFailuresInAllThreePhases.value) = 8;
    }
    return ret;
}

int addeventCode1()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 96, 11, 0, 255 };
    if ((ret = INIT_OBJECT(eventCode1, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(eventCode1.value) = 0;
    }
    return ret;
}

int addeventCurrentRelated()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 96, 11, 1, 255 };
    if ((ret = INIT_OBJECT(eventCurrentRelated, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(eventCurrentRelated.value) = 0;
    }
    return ret;
}

int addeventPowerRelated()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 96, 11, 2, 255 };
    if ((ret = INIT_OBJECT(eventPowerRelated, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(eventPowerRelated.value) = 102;
    }
    return ret;
}

int addeventTransactionRelated()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 96, 11, 3, 255 };
    if ((ret = INIT_OBJECT(eventTransactionRelated, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(eventTransactionRelated.value) = 0;
    }
    return ret;
}

int addeventOthers()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 96, 11, 4, 255 };
    if ((ret = INIT_OBJECT(eventOthers, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(eventOthers.value) = 0;
    }
    return ret;
}

int addeventNonRollOverEvents()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 96, 11, 5, 255 };
    if ((ret = INIT_OBJECT(eventNonRollOverEvents, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(eventNonRollOverEvents.value) = 0;
    }
    return ret;
}

int addeventLoadSwitchStatus()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 96, 11, 6, 255 };
    if ((ret = INIT_OBJECT(eventLoadSwitchStatus, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(eventLoadSwitchStatus.value) = 0;
    }
    return ret;
}

int addfirmwareVersionForMeter()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 0, 2, 0, 255 };
    if ((ret = INIT_OBJECT(firmwareVersionForMeter, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        static char data[131];


        GX_OCTECT_STRING(firmwareVersionForMeter.value, data, 13);
    }
    return ret;
}

int addtransformerRatioMinusCurrent()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 0, 4, 2, 255 };
    if ((ret = INIT_OBJECT(transformerRatioMinusCurrent, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(transformerRatioMinusCurrent.value) = 1;
    }
    return ret;
}

int addtransformerRatioMinusVoltage()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 0, 4, 3, 255 };
    if ((ret = INIT_OBJECT(transformerRatioMinusVoltage, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(transformerRatioMinusVoltage.value) = 1;
    }
    return ret;
}

int adddemandIntegrationPeriod()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 0, 8, 0, 255 };
    if ((ret = INIT_OBJECT(demandIntegrationPeriod, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(demandIntegrationPeriod.value) = 1800;
    }
    return ret;
}

int addprofileCapturePeriod()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 0, 8, 4, 255 };
    if ((ret = INIT_OBJECT(profileCapturePeriod, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(profileCapturePeriod.value) = 1800;
    }
    return ret;
}

int addrecordingInterval2ForLoadProfile()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 0, 8, 5, 255 };
    if ((ret = INIT_OBJECT(recordingInterval2ForLoadProfile, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT32(recordingInterval2ForLoadProfile.value) = 86400;
    }
    return ret;
}

int addtimeStampOfTheMostRecentBillingPeriodClosed()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 0, 1, 2, 255 };
    if ((ret = INIT_OBJECT(timeStampOfTheMostRecentBillingPeriodClosed, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {

    }
    return ret;
}

int addmaximumDemandKw()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 1, 6, 0, 255 };
    if ((ret = INIT_OBJECT(maximumDemandKw, DLMS_OBJECT_TYPE_EXTENDED_REGISTER, ln)) == 0)
    {
        GX_UINT16(maximumDemandKw.value) = 25400;
        //10 ^ 3 =  1000
        maximumDemandKw.scaler = -2;
        maximumDemandKw.unit = DLMS_UNIT_ACTIVE_ENERGY;
    }
    return ret;
}

int addsumLiActivePowerMinusMax1RateIsTotal()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 2, 6, 0, 255 };
    if ((ret = INIT_OBJECT(sumLiActivePowerMinusMax1RateIsTotal, DLMS_OBJECT_TYPE_EXTENDED_REGISTER, ln)) == 0)
    {
        GX_UINT16(sumLiActivePowerMinusMax1RateIsTotal.value) = 25400;
        //10 ^ 3 =  1000
        sumLiActivePowerMinusMax1RateIsTotal.scaler = -2;
        sumLiActivePowerMinusMax1RateIsTotal.unit = DLMS_UNIT_ACTIVE_ENERGY;
    }
    return ret;
}

int addmaximumDemandKva()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 9, 6, 0, 255 };
    if ((ret = INIT_OBJECT(maximumDemandKva, DLMS_OBJECT_TYPE_EXTENDED_REGISTER, ln)) == 0)
    {
        GX_UINT16(maximumDemandKva.value) = 25400;
        //10 ^ 3 =  1000
        maximumDemandKva.scaler = -2;
        maximumDemandKva.unit = DLMS_UNIT_ACTIVE_ENERGY;
    }
    return ret;
}

int addsumLiApparentPowerMinusMax1RateIsTotal()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 10, 6, 0, 255 };
    if ((ret = INIT_OBJECT(sumLiApparentPowerMinusMax1RateIsTotal, DLMS_OBJECT_TYPE_EXTENDED_REGISTER, ln)) == 0)
    {
        GX_UINT16(sumLiApparentPowerMinusMax1RateIsTotal.value) = 25400;
        //10 ^ 3 =  1000
        sumLiApparentPowerMinusMax1RateIsTotal.scaler = -2;
        sumLiApparentPowerMinusMax1RateIsTotal.unit = DLMS_UNIT_ACTIVE_ENERGY;
    }
    return ret;
}

int addiecHdlcSetup()
{
    int ret;
    unsigned char ln[6] = { 0, 0, 22, 0, 0, 255 };
    if ((ret = INIT_OBJECT(iecHdlcSetup, DLMS_OBJECT_TYPE_IEC_HDLC_SETUP, ln)) == 0)
    {
        iecHdlcSetup.communicationSpeed = DLMS_BAUD_RATE_9600;
        iecHdlcSetup.windowSizeReceive = iecHdlcSetup.windowSizeTransmit = 1;
        iecHdlcSetup.maximumInfoLengthTransmit = iecHdlcSetup.maximumInfoLengthReceive = 128;
        iecHdlcSetup.inactivityTimeout = 120;
        iecHdlcSetup.deviceAddress = 0x10;
    }
    return ret;
}

int addvoltageRelatedEventsProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[12];
    const unsigned char ln[6] = { 0, 0, 99, 98, 0, 255 };
    if ((ret = INIT_OBJECT(voltageRelatedEventsProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(voltageRelatedEventsProfile.captureObjects, CAPTURE_OBJECT, 12);
        //Set capture period to time in seconds.
        voltageRelatedEventsProfile.capturePeriod = 0;
        //Maximum row count.
        voltageRelatedEventsProfile.profileEntries = 70;
        voltageRelatedEventsProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(clock1);
        CAPTURE_OBJECT[0].attributeIndex = 2;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(eventCode1);
        CAPTURE_OBJECT[1].attributeIndex = 2;
        CAPTURE_OBJECT[1].dataIndex = 0;
        CAPTURE_OBJECT[2].target = BASE(l1CurrentInstValue);
        CAPTURE_OBJECT[2].attributeIndex = 2;
        CAPTURE_OBJECT[2].dataIndex = 0;
        CAPTURE_OBJECT[3].target = BASE(l2CurrentInstValue);
        CAPTURE_OBJECT[3].attributeIndex = 2;
        CAPTURE_OBJECT[3].dataIndex = 0;
        CAPTURE_OBJECT[4].target = BASE(l3CurrentInstValue);
        CAPTURE_OBJECT[4].attributeIndex = 2;
        CAPTURE_OBJECT[4].dataIndex = 0;
        CAPTURE_OBJECT[5].target = BASE(l1VoltageInstValue);
        CAPTURE_OBJECT[5].attributeIndex = 2;
        CAPTURE_OBJECT[5].dataIndex = 0;
        CAPTURE_OBJECT[6].target = BASE(l2VoltageInstValue);
        CAPTURE_OBJECT[6].attributeIndex = 2;
        CAPTURE_OBJECT[6].dataIndex = 0;
        CAPTURE_OBJECT[7].target = BASE(l3VoltageInstValue);
        CAPTURE_OBJECT[7].attributeIndex = 2;
        CAPTURE_OBJECT[7].dataIndex = 0;
        CAPTURE_OBJECT[8].target = BASE(l1PowerFactorInstValue);
        CAPTURE_OBJECT[8].attributeIndex = 2;
        CAPTURE_OBJECT[8].dataIndex = 0;
        CAPTURE_OBJECT[9].target = BASE(l2PowerFactorInstValue);
        CAPTURE_OBJECT[9].attributeIndex = 2;
        CAPTURE_OBJECT[9].dataIndex = 0;
        CAPTURE_OBJECT[10].target = BASE(l3PowerFactorInstValue);
        CAPTURE_OBJECT[10].attributeIndex = 2;
        CAPTURE_OBJECT[10].dataIndex = 0;
        CAPTURE_OBJECT[11].target = BASE(cumulativeEnergyKwh);
        CAPTURE_OBJECT[11].attributeIndex = 2;
        CAPTURE_OBJECT[11].dataIndex = 0;

    }
    return ret;
}

int addcurrentRelatedEventsProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[12];
    const unsigned char ln[6] = { 0, 0, 99, 98, 1, 255 };
    if ((ret = INIT_OBJECT(currentRelatedEventsProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(currentRelatedEventsProfile.captureObjects, CAPTURE_OBJECT, 12);
        //Set capture period to time in seconds.
        currentRelatedEventsProfile.capturePeriod = 0;
        //Maximum row count.
        currentRelatedEventsProfile.profileEntries = 70;
        currentRelatedEventsProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(clock1);
        CAPTURE_OBJECT[0].attributeIndex = 2;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(eventCurrentRelated);
        CAPTURE_OBJECT[1].attributeIndex = 2;
        CAPTURE_OBJECT[1].dataIndex = 0;
        CAPTURE_OBJECT[2].target = BASE(l1CurrentInstValue);
        CAPTURE_OBJECT[2].attributeIndex = 2;
        CAPTURE_OBJECT[2].dataIndex = 0;
        CAPTURE_OBJECT[3].target = BASE(l2CurrentInstValue);
        CAPTURE_OBJECT[3].attributeIndex = 2;
        CAPTURE_OBJECT[3].dataIndex = 0;
        CAPTURE_OBJECT[4].target = BASE(l3CurrentInstValue);
        CAPTURE_OBJECT[4].attributeIndex = 2;
        CAPTURE_OBJECT[4].dataIndex = 0;
        CAPTURE_OBJECT[5].target = BASE(l1VoltageInstValue);
        CAPTURE_OBJECT[5].attributeIndex = 2;
        CAPTURE_OBJECT[5].dataIndex = 0;
        CAPTURE_OBJECT[6].target = BASE(l2VoltageInstValue);
        CAPTURE_OBJECT[6].attributeIndex = 2;
        CAPTURE_OBJECT[6].dataIndex = 0;
        CAPTURE_OBJECT[7].target = BASE(l3VoltageInstValue);
        CAPTURE_OBJECT[7].attributeIndex = 2;
        CAPTURE_OBJECT[7].dataIndex = 0;
        CAPTURE_OBJECT[8].target = BASE(l1PowerFactorInstValue);
        CAPTURE_OBJECT[8].attributeIndex = 2;
        CAPTURE_OBJECT[8].dataIndex = 0;
        CAPTURE_OBJECT[9].target = BASE(l2PowerFactorInstValue);
        CAPTURE_OBJECT[9].attributeIndex = 2;
        CAPTURE_OBJECT[9].dataIndex = 0;
        CAPTURE_OBJECT[10].target = BASE(l3PowerFactorInstValue);
        CAPTURE_OBJECT[10].attributeIndex = 2;
        CAPTURE_OBJECT[10].dataIndex = 0;
        CAPTURE_OBJECT[11].target = BASE(cumulativeEnergyKwh);
        CAPTURE_OBJECT[11].attributeIndex = 2;
        CAPTURE_OBJECT[11].dataIndex = 0;

    }
    return ret;
}

int addpowerRelatedEventsProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[2];
    const unsigned char ln[6] = { 0, 0, 99, 98, 2, 255 };
    if ((ret = INIT_OBJECT(powerRelatedEventsProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(powerRelatedEventsProfile.captureObjects, CAPTURE_OBJECT, 2);
        //Set capture period to time in seconds.
        powerRelatedEventsProfile.capturePeriod = 0;
        //Maximum row count.
        powerRelatedEventsProfile.profileEntries = 40;
        powerRelatedEventsProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(clock1);
        CAPTURE_OBJECT[0].attributeIndex = 2;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(eventPowerRelated);
        CAPTURE_OBJECT[1].attributeIndex = 2;
        CAPTURE_OBJECT[1].dataIndex = 0;

    }
    return ret;
}

int addtransactionEventsProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[2];
    const unsigned char ln[6] = { 0, 0, 99, 98, 3, 255 };
    if ((ret = INIT_OBJECT(transactionEventsProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(transactionEventsProfile.captureObjects, CAPTURE_OBJECT, 2);
        //Set capture period to time in seconds.
        transactionEventsProfile.capturePeriod = 0;
        //Maximum row count.
        transactionEventsProfile.profileEntries = 20;
        transactionEventsProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(clock1);
        CAPTURE_OBJECT[0].attributeIndex = 2;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(eventTransactionRelated);
        CAPTURE_OBJECT[1].attributeIndex = 2;
        CAPTURE_OBJECT[1].dataIndex = 0;

    }
    return ret;
}

int addotherTamperEventsProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[12];
    const unsigned char ln[6] = { 0, 0, 99, 98, 4, 255 };
    if ((ret = INIT_OBJECT(otherTamperEventsProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(otherTamperEventsProfile.captureObjects, CAPTURE_OBJECT, 12);
        //Set capture period to time in seconds.
        otherTamperEventsProfile.capturePeriod = 0;
        //Maximum row count.
        otherTamperEventsProfile.profileEntries = 60;
        otherTamperEventsProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(clock1);
        CAPTURE_OBJECT[0].attributeIndex = 2;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(eventOthers);
        CAPTURE_OBJECT[1].attributeIndex = 2;
        CAPTURE_OBJECT[1].dataIndex = 0;
        CAPTURE_OBJECT[2].target = BASE(l1CurrentInstValue);
        CAPTURE_OBJECT[2].attributeIndex = 2;
        CAPTURE_OBJECT[2].dataIndex = 0;
        CAPTURE_OBJECT[3].target = BASE(l2CurrentInstValue);
        CAPTURE_OBJECT[3].attributeIndex = 2;
        CAPTURE_OBJECT[3].dataIndex = 0;
        CAPTURE_OBJECT[4].target = BASE(l3CurrentInstValue);
        CAPTURE_OBJECT[4].attributeIndex = 2;
        CAPTURE_OBJECT[4].dataIndex = 0;
        CAPTURE_OBJECT[5].target = BASE(l1VoltageInstValue);
        CAPTURE_OBJECT[5].attributeIndex = 2;
        CAPTURE_OBJECT[5].dataIndex = 0;
        CAPTURE_OBJECT[6].target = BASE(l2VoltageInstValue);
        CAPTURE_OBJECT[6].attributeIndex = 2;
        CAPTURE_OBJECT[6].dataIndex = 0;
        CAPTURE_OBJECT[7].target = BASE(l3VoltageInstValue);
        CAPTURE_OBJECT[7].attributeIndex = 2;
        CAPTURE_OBJECT[7].dataIndex = 0;
        CAPTURE_OBJECT[8].target = BASE(l1PowerFactorInstValue);
        CAPTURE_OBJECT[8].attributeIndex = 2;
        CAPTURE_OBJECT[8].dataIndex = 0;
        CAPTURE_OBJECT[9].target = BASE(l2PowerFactorInstValue);
        CAPTURE_OBJECT[9].attributeIndex = 2;
        CAPTURE_OBJECT[9].dataIndex = 0;
        CAPTURE_OBJECT[10].target = BASE(l3PowerFactorInstValue);
        CAPTURE_OBJECT[10].attributeIndex = 2;
        CAPTURE_OBJECT[10].dataIndex = 0;
        CAPTURE_OBJECT[11].target = BASE(cumulativeEnergyKwh);
        CAPTURE_OBJECT[11].attributeIndex = 2;
        CAPTURE_OBJECT[11].dataIndex = 0;

    }
    return ret;
}

int addnonRollOverEventsProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[2];
    const unsigned char ln[6] = { 0, 0, 99, 98, 5, 255 };
    if ((ret = INIT_OBJECT(nonRollOverEventsProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(nonRollOverEventsProfile.captureObjects, CAPTURE_OBJECT, 2);
        //Set capture period to time in seconds.
        nonRollOverEventsProfile.capturePeriod = 0;
        //Maximum row count.
        nonRollOverEventsProfile.profileEntries = 1;
        nonRollOverEventsProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(clock1);
        CAPTURE_OBJECT[0].attributeIndex = 2;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(eventNonRollOverEvents);
        CAPTURE_OBJECT[1].attributeIndex = 2;
        CAPTURE_OBJECT[1].dataIndex = 0;

    }
    return ret;
}

int addcontrolEventsProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[2];
    const unsigned char ln[6] = { 0, 0, 99, 98, 6, 255 };
    if ((ret = INIT_OBJECT(controlEventsProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(controlEventsProfile.captureObjects, CAPTURE_OBJECT, 2);
        //Set capture period to time in seconds.
        controlEventsProfile.capturePeriod = 0;
        //Maximum row count.
        controlEventsProfile.profileEntries = 1;
        controlEventsProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(clock1);
        CAPTURE_OBJECT[0].attributeIndex = 2;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(eventLoadSwitchStatus);
        CAPTURE_OBJECT[1].attributeIndex = 2;
        CAPTURE_OBJECT[1].dataIndex = 0;

    }
    return ret;
}

int addinstantaneousProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[26];
    const unsigned char ln[6] = { 1, 0, 94, 91, 0, 255 };
    if ((ret = INIT_OBJECT(instantaneousProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(instantaneousProfile.captureObjects, CAPTURE_OBJECT, 26);
        //Set capture period to time in seconds.
        instantaneousProfile.capturePeriod = 0;
        //Maximum row count.
        instantaneousProfile.profileEntries = 1;
        instantaneousProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(clock1);
        CAPTURE_OBJECT[0].attributeIndex = 2;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(l1CurrentInstValue);
        CAPTURE_OBJECT[1].attributeIndex = 2;
        CAPTURE_OBJECT[1].dataIndex = 0;
        CAPTURE_OBJECT[2].target = BASE(l2CurrentInstValue);
        CAPTURE_OBJECT[2].attributeIndex = 2;
        CAPTURE_OBJECT[2].dataIndex = 0;
        CAPTURE_OBJECT[3].target = BASE(l3CurrentInstValue);
        CAPTURE_OBJECT[3].attributeIndex = 2;
        CAPTURE_OBJECT[3].dataIndex = 0;
        CAPTURE_OBJECT[4].target = BASE(l1VoltageInstValue);
        CAPTURE_OBJECT[4].attributeIndex = 2;
        CAPTURE_OBJECT[4].dataIndex = 0;
        CAPTURE_OBJECT[5].target = BASE(l2VoltageInstValue);
        CAPTURE_OBJECT[5].attributeIndex = 2;
        CAPTURE_OBJECT[5].dataIndex = 0;
        CAPTURE_OBJECT[6].target = BASE(l3VoltageInstValue);
        CAPTURE_OBJECT[6].attributeIndex = 2;
        CAPTURE_OBJECT[6].dataIndex = 0;
        CAPTURE_OBJECT[7].target = BASE(l1PowerFactorInstValue);
        CAPTURE_OBJECT[7].attributeIndex = 2;
        CAPTURE_OBJECT[7].dataIndex = 0;
        CAPTURE_OBJECT[8].target = BASE(l2PowerFactorInstValue);
        CAPTURE_OBJECT[8].attributeIndex = 2;
        CAPTURE_OBJECT[8].dataIndex = 0;
        CAPTURE_OBJECT[9].target = BASE(l3PowerFactorInstValue);
        CAPTURE_OBJECT[9].attributeIndex = 2;
        CAPTURE_OBJECT[9].dataIndex = 0;
        CAPTURE_OBJECT[10].target = BASE(signedPowerFactor);
        CAPTURE_OBJECT[10].attributeIndex = 2;
        CAPTURE_OBJECT[10].dataIndex = 0;
        CAPTURE_OBJECT[11].target = BASE(frequencyHz);
        CAPTURE_OBJECT[11].attributeIndex = 2;
        CAPTURE_OBJECT[11].dataIndex = 0;
        CAPTURE_OBJECT[12].target = BASE(apparentPowerKva);
        CAPTURE_OBJECT[12].attributeIndex = 2;
        CAPTURE_OBJECT[12].dataIndex = 0;
        CAPTURE_OBJECT[13].target = BASE(activePowerKw);
        CAPTURE_OBJECT[13].attributeIndex = 2;
        CAPTURE_OBJECT[13].dataIndex = 0;
        CAPTURE_OBJECT[14].target = BASE(sumLiReactivePowerPlusInstValue);
        CAPTURE_OBJECT[14].attributeIndex = 2;
        CAPTURE_OBJECT[14].dataIndex = 0;
        CAPTURE_OBJECT[15].target = BASE(cumulativeEnergyKwh);
        CAPTURE_OBJECT[15].attributeIndex = 2;
        CAPTURE_OBJECT[15].dataIndex = 0;
        CAPTURE_OBJECT[16].target = BASE(cumulativeEnergyKwhExport);
        CAPTURE_OBJECT[16].attributeIndex = 2;
        CAPTURE_OBJECT[16].dataIndex = 0;
        CAPTURE_OBJECT[17].target = BASE(cumulativeEnergyKvahImport);
        CAPTURE_OBJECT[17].attributeIndex = 2;
        CAPTURE_OBJECT[17].dataIndex = 0;
        CAPTURE_OBJECT[18].target = BASE(cumulativeEnergyKvahExport);
        CAPTURE_OBJECT[18].attributeIndex = 2;
        CAPTURE_OBJECT[18].dataIndex = 0;
        CAPTURE_OBJECT[19].target = BASE(noOfPowerFailuresInAllThreePhases);
        CAPTURE_OBJECT[19].attributeIndex = 2;
        CAPTURE_OBJECT[19].dataIndex = 0;
        CAPTURE_OBJECT[20].target = BASE(cumulativePowerOffDurationInMin);
        CAPTURE_OBJECT[20].attributeIndex = 2;
        CAPTURE_OBJECT[20].dataIndex = 0;
        CAPTURE_OBJECT[21].target = BASE(cumulativeTamperCount);
        CAPTURE_OBJECT[21].attributeIndex = 2;
        CAPTURE_OBJECT[21].dataIndex = 0;
        CAPTURE_OBJECT[22].target = BASE(cumulativeBillingCount);
        CAPTURE_OBJECT[22].attributeIndex = 2;
        CAPTURE_OBJECT[22].dataIndex = 0;
        CAPTURE_OBJECT[23].target = BASE(cumulativeProgrammingCount);
        CAPTURE_OBJECT[23].attributeIndex = 2;
        CAPTURE_OBJECT[23].dataIndex = 0;
        CAPTURE_OBJECT[24].target = BASE(billingDateImportMode);
        CAPTURE_OBJECT[24].attributeIndex = 2;
        CAPTURE_OBJECT[24].dataIndex = 0;
        CAPTURE_OBJECT[25].target = BASE(sumLiActivePowerTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[25].attributeIndex = 2;
        CAPTURE_OBJECT[25].dataIndex = 0;

    }
    return ret;
}

int addscalerInstantaneousProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[21];
    const unsigned char ln[6] = { 1, 0, 94, 91, 3, 255 };
    if ((ret = INIT_OBJECT(scalerInstantaneousProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(scalerInstantaneousProfile.captureObjects, CAPTURE_OBJECT, 21);
        //Set capture period to time in seconds.
        scalerInstantaneousProfile.capturePeriod = 0;
        //Maximum row count.
        scalerInstantaneousProfile.profileEntries = 1;
        scalerInstantaneousProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(l1CurrentInstValue);
        CAPTURE_OBJECT[0].attributeIndex = 3;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(l2CurrentInstValue);
        CAPTURE_OBJECT[1].attributeIndex = 3;
        CAPTURE_OBJECT[1].dataIndex = 0;
        CAPTURE_OBJECT[2].target = BASE(l3CurrentInstValue);
        CAPTURE_OBJECT[2].attributeIndex = 3;
        CAPTURE_OBJECT[2].dataIndex = 0;
        CAPTURE_OBJECT[3].target = BASE(l1VoltageInstValue);
        CAPTURE_OBJECT[3].attributeIndex = 3;
        CAPTURE_OBJECT[3].dataIndex = 0;
        CAPTURE_OBJECT[4].target = BASE(l2VoltageInstValue);
        CAPTURE_OBJECT[4].attributeIndex = 3;
        CAPTURE_OBJECT[4].dataIndex = 0;
        CAPTURE_OBJECT[5].target = BASE(l3VoltageInstValue);
        CAPTURE_OBJECT[5].attributeIndex = 3;
        CAPTURE_OBJECT[5].dataIndex = 0;
        CAPTURE_OBJECT[6].target = BASE(l1PowerFactorInstValue);
        CAPTURE_OBJECT[6].attributeIndex = 3;
        CAPTURE_OBJECT[6].dataIndex = 0;
        CAPTURE_OBJECT[7].target = BASE(l2PowerFactorInstValue);
        CAPTURE_OBJECT[7].attributeIndex = 3;
        CAPTURE_OBJECT[7].dataIndex = 0;
        CAPTURE_OBJECT[8].target = BASE(l3PowerFactorInstValue);
        CAPTURE_OBJECT[8].attributeIndex = 3;
        CAPTURE_OBJECT[8].dataIndex = 0;
        CAPTURE_OBJECT[9].target = BASE(signedPowerFactor);
        CAPTURE_OBJECT[9].attributeIndex = 3;
        CAPTURE_OBJECT[9].dataIndex = 0;
        CAPTURE_OBJECT[10].target = BASE(frequencyHz);
        CAPTURE_OBJECT[10].attributeIndex = 3;
        CAPTURE_OBJECT[10].dataIndex = 0;
        CAPTURE_OBJECT[11].target = BASE(apparentPowerKva);
        CAPTURE_OBJECT[11].attributeIndex = 3;
        CAPTURE_OBJECT[11].dataIndex = 0;
        CAPTURE_OBJECT[12].target = BASE(activePowerKw);
        CAPTURE_OBJECT[12].attributeIndex = 3;
        CAPTURE_OBJECT[12].dataIndex = 0;
        CAPTURE_OBJECT[13].target = BASE(sumLiReactivePowerPlusInstValue);
        CAPTURE_OBJECT[13].attributeIndex = 3;
        CAPTURE_OBJECT[13].dataIndex = 0;
        CAPTURE_OBJECT[14].target = BASE(cumulativeEnergyKwh);
        CAPTURE_OBJECT[14].attributeIndex = 3;
        CAPTURE_OBJECT[14].dataIndex = 0;
        CAPTURE_OBJECT[15].target = BASE(cumulativeEnergyKwhExport);
        CAPTURE_OBJECT[15].attributeIndex = 3;
        CAPTURE_OBJECT[15].dataIndex = 0;
        CAPTURE_OBJECT[16].target = BASE(cumulativeEnergyKvahImport);
        CAPTURE_OBJECT[16].attributeIndex = 3;
        CAPTURE_OBJECT[16].dataIndex = 0;
        CAPTURE_OBJECT[17].target = BASE(cumulativeEnergyKvahExport);
        CAPTURE_OBJECT[17].attributeIndex = 3;
        CAPTURE_OBJECT[17].dataIndex = 0;
        CAPTURE_OBJECT[18].target = BASE(cumulativePowerOffDurationInMin);
        CAPTURE_OBJECT[18].attributeIndex = 3;
        CAPTURE_OBJECT[18].dataIndex = 0;
        CAPTURE_OBJECT[19].target = BASE(billingDateImportMode);
        CAPTURE_OBJECT[19].attributeIndex = 3;
        CAPTURE_OBJECT[19].dataIndex = 0;
        CAPTURE_OBJECT[20].target = BASE(sumLiActivePowerTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[20].attributeIndex = 3;
        CAPTURE_OBJECT[20].dataIndex = 0;

    }
    return ret;
}

int addscalerBlockLoadProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[15];
    const unsigned char ln[6] = { 1, 0, 94, 91, 4, 255 };
    if ((ret = INIT_OBJECT(scalerBlockLoadProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(scalerBlockLoadProfile.captureObjects, CAPTURE_OBJECT, 15);
        //Set capture period to time in seconds.
        scalerBlockLoadProfile.capturePeriod = 0;
        //Maximum row count.
        scalerBlockLoadProfile.profileEntries = 1;
        scalerBlockLoadProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(l1CurrentCurrentAvg5);
        CAPTURE_OBJECT[0].attributeIndex = 3;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(l2CurrentCurrentAvg5);
        CAPTURE_OBJECT[1].attributeIndex = 3;
        CAPTURE_OBJECT[1].dataIndex = 0;
        CAPTURE_OBJECT[2].target = BASE(l3CurrentCurrentAvg5);
        CAPTURE_OBJECT[2].attributeIndex = 3;
        CAPTURE_OBJECT[2].dataIndex = 0;
        CAPTURE_OBJECT[3].target = BASE(l1VoltageCurrentAvg5);
        CAPTURE_OBJECT[3].attributeIndex = 3;
        CAPTURE_OBJECT[3].dataIndex = 0;
        CAPTURE_OBJECT[4].target = BASE(l2VoltageCurrentAvg5);
        CAPTURE_OBJECT[4].attributeIndex = 3;
        CAPTURE_OBJECT[4].dataIndex = 0;
        CAPTURE_OBJECT[5].target = BASE(l3VoltageCurrentAvg5);
        CAPTURE_OBJECT[5].attributeIndex = 3;
        CAPTURE_OBJECT[5].dataIndex = 0;
        CAPTURE_OBJECT[6].target = BASE(blockEnergyKwhImport);
        CAPTURE_OBJECT[6].attributeIndex = 3;
        CAPTURE_OBJECT[6].dataIndex = 0;
        CAPTURE_OBJECT[7].target = BASE(blockEnergyKwhExport);
        CAPTURE_OBJECT[7].attributeIndex = 3;
        CAPTURE_OBJECT[7].dataIndex = 0;
        CAPTURE_OBJECT[8].target = BASE(sumLiActivePowerTimeIntegral5);
        CAPTURE_OBJECT[8].attributeIndex = 3;
        CAPTURE_OBJECT[8].dataIndex = 0;
        CAPTURE_OBJECT[9].target = BASE(blockEnergyKvahImport);
        CAPTURE_OBJECT[9].attributeIndex = 3;
        CAPTURE_OBJECT[9].dataIndex = 0;
        CAPTURE_OBJECT[10].target = BASE(blockEnergyKvahExport);
        CAPTURE_OBJECT[10].attributeIndex = 3;
        CAPTURE_OBJECT[10].dataIndex = 0;
        CAPTURE_OBJECT[11].target = BASE(sumLiReactivePowerQiTimeIntegral5);
        CAPTURE_OBJECT[11].attributeIndex = 3;
        CAPTURE_OBJECT[11].dataIndex = 0;
        CAPTURE_OBJECT[12].target = BASE(sumLiReactivePowerQiiTimeIntegral5);
        CAPTURE_OBJECT[12].attributeIndex = 3;
        CAPTURE_OBJECT[12].dataIndex = 0;
        CAPTURE_OBJECT[13].target = BASE(sumLiReactivePowerQiiiTimeIntegral5);
        CAPTURE_OBJECT[13].attributeIndex = 3;
        CAPTURE_OBJECT[13].dataIndex = 0;
        CAPTURE_OBJECT[14].target = BASE(sumLiReactivePowerQivTimeIntegral5);
        CAPTURE_OBJECT[14].attributeIndex = 3;
        CAPTURE_OBJECT[14].dataIndex = 0;

    }
    return ret;
}

int addscalerDailyLoadProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[9];
    const unsigned char ln[6] = { 1, 0, 94, 91, 5, 255 };
    if ((ret = INIT_OBJECT(scalerDailyLoadProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(scalerDailyLoadProfile.captureObjects, CAPTURE_OBJECT, 9);
        //Set capture period to time in seconds.
        scalerDailyLoadProfile.capturePeriod = 0;
        //Maximum row count.
        scalerDailyLoadProfile.profileEntries = 1;
        scalerDailyLoadProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(cumulativeEnergyKwh);
        CAPTURE_OBJECT[0].attributeIndex = 3;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(cumulativeEnergyKwhExport);
        CAPTURE_OBJECT[1].attributeIndex = 3;
        CAPTURE_OBJECT[1].dataIndex = 0;
        CAPTURE_OBJECT[2].target = BASE(cumulativeEnergyKvahImport);
        CAPTURE_OBJECT[2].attributeIndex = 3;
        CAPTURE_OBJECT[2].dataIndex = 0;
        CAPTURE_OBJECT[3].target = BASE(cumulativeEnergyKvahExport);
        CAPTURE_OBJECT[3].attributeIndex = 3;
        CAPTURE_OBJECT[3].dataIndex = 0;
        CAPTURE_OBJECT[4].target = BASE(sumLiReactivePowerQiTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[4].attributeIndex = 3;
        CAPTURE_OBJECT[4].dataIndex = 0;
        CAPTURE_OBJECT[5].target = BASE(sumLiReactivePowerQiiTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[5].attributeIndex = 3;
        CAPTURE_OBJECT[5].dataIndex = 0;
        CAPTURE_OBJECT[6].target = BASE(sumLiReactivePowerQiiiTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[6].attributeIndex = 3;
        CAPTURE_OBJECT[6].dataIndex = 0;
        CAPTURE_OBJECT[7].target = BASE(sumLiReactivePowerQivTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[7].attributeIndex = 3;
        CAPTURE_OBJECT[7].dataIndex = 0;
        CAPTURE_OBJECT[8].target = BASE(sumLiActivePowerTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[8].attributeIndex = 3;
        CAPTURE_OBJECT[8].dataIndex = 0;

    }
    return ret;
}

int addscalerBillingProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[14];
    const unsigned char ln[6] = { 1, 0, 94, 91, 6, 255 };
    if ((ret = INIT_OBJECT(scalerBillingProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(scalerBillingProfile.captureObjects, CAPTURE_OBJECT, 14);
        //Set capture period to time in seconds.
        scalerBillingProfile.capturePeriod = 0;
        //Maximum row count.
        scalerBillingProfile.profileEntries = 1;
        scalerBillingProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(cumulativeEnergyKwh);
        CAPTURE_OBJECT[0].attributeIndex = 3;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(cumulativeEnergyKwhExport);
        CAPTURE_OBJECT[1].attributeIndex = 3;
        CAPTURE_OBJECT[1].dataIndex = 0;
        CAPTURE_OBJECT[2].target = BASE(cumulativeEnergyKvahImport);
        CAPTURE_OBJECT[2].attributeIndex = 3;
        CAPTURE_OBJECT[2].dataIndex = 0;
        CAPTURE_OBJECT[3].target = BASE(cumulativeEnergyKvahExport);
        CAPTURE_OBJECT[3].attributeIndex = 3;
        CAPTURE_OBJECT[3].dataIndex = 0;
        CAPTURE_OBJECT[4].target = BASE(sumLiReactivePowerQiTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[4].attributeIndex = 3;
        CAPTURE_OBJECT[4].dataIndex = 0;
        CAPTURE_OBJECT[5].target = BASE(sumLiReactivePowerQiiTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[5].attributeIndex = 3;
        CAPTURE_OBJECT[5].dataIndex = 0;
        CAPTURE_OBJECT[6].target = BASE(sumLiReactivePowerQiiiTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[6].attributeIndex = 3;
        CAPTURE_OBJECT[6].dataIndex = 0;
        CAPTURE_OBJECT[7].target = BASE(sumLiReactivePowerQivTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[7].attributeIndex = 3;
        CAPTURE_OBJECT[7].dataIndex = 0;
        CAPTURE_OBJECT[8].target = BASE(maximumDemandKw);
        CAPTURE_OBJECT[8].attributeIndex = 3;
        CAPTURE_OBJECT[8].dataIndex = 0;
        CAPTURE_OBJECT[9].target = BASE(sumLiActivePowerMinusMax1RateIsTotal);
        CAPTURE_OBJECT[9].attributeIndex = 3;
        CAPTURE_OBJECT[9].dataIndex = 0;
        CAPTURE_OBJECT[10].target = BASE(maximumDemandKva);
        CAPTURE_OBJECT[10].attributeIndex = 3;
        CAPTURE_OBJECT[10].dataIndex = 0;
        CAPTURE_OBJECT[11].target = BASE(sumLiApparentPowerMinusMax1RateIsTotal);
        CAPTURE_OBJECT[11].attributeIndex = 3;
        CAPTURE_OBJECT[11].dataIndex = 0;
        CAPTURE_OBJECT[12].target = BASE(sumLiActivePowerTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[12].attributeIndex = 3;
        CAPTURE_OBJECT[12].dataIndex = 0;
        CAPTURE_OBJECT[13].target = BASE(identifiersForIndia);
        CAPTURE_OBJECT[13].attributeIndex = 3;
        CAPTURE_OBJECT[13].dataIndex = 0;

    }
    return ret;
}

int addscalerEventsProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[10];
    const unsigned char ln[6] = { 1, 0, 94, 91, 7, 255 };
    if ((ret = INIT_OBJECT(scalerEventsProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(scalerEventsProfile.captureObjects, CAPTURE_OBJECT, 10);
        //Set capture period to time in seconds.
        scalerEventsProfile.capturePeriod = 0;
        //Maximum row count.
        scalerEventsProfile.profileEntries = 1;
        scalerEventsProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(l1CurrentInstValue);
        CAPTURE_OBJECT[0].attributeIndex = 3;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(l2CurrentInstValue);
        CAPTURE_OBJECT[1].attributeIndex = 3;
        CAPTURE_OBJECT[1].dataIndex = 0;
        CAPTURE_OBJECT[2].target = BASE(l3CurrentInstValue);
        CAPTURE_OBJECT[2].attributeIndex = 3;
        CAPTURE_OBJECT[2].dataIndex = 0;
        CAPTURE_OBJECT[3].target = BASE(l1VoltageInstValue);
        CAPTURE_OBJECT[3].attributeIndex = 3;
        CAPTURE_OBJECT[3].dataIndex = 0;
        CAPTURE_OBJECT[4].target = BASE(l2VoltageInstValue);
        CAPTURE_OBJECT[4].attributeIndex = 3;
        CAPTURE_OBJECT[4].dataIndex = 0;
        CAPTURE_OBJECT[5].target = BASE(l3VoltageInstValue);
        CAPTURE_OBJECT[5].attributeIndex = 3;
        CAPTURE_OBJECT[5].dataIndex = 0;
        CAPTURE_OBJECT[6].target = BASE(l1PowerFactorInstValue);
        CAPTURE_OBJECT[6].attributeIndex = 3;
        CAPTURE_OBJECT[6].dataIndex = 0;
        CAPTURE_OBJECT[7].target = BASE(l2PowerFactorInstValue);
        CAPTURE_OBJECT[7].attributeIndex = 3;
        CAPTURE_OBJECT[7].dataIndex = 0;
        CAPTURE_OBJECT[8].target = BASE(l3PowerFactorInstValue);
        CAPTURE_OBJECT[8].attributeIndex = 3;
        CAPTURE_OBJECT[8].dataIndex = 0;
        CAPTURE_OBJECT[9].target = BASE(cumulativeEnergyKwh);
        CAPTURE_OBJECT[9].attributeIndex = 3;
        CAPTURE_OBJECT[9].dataIndex = 0;

    }
    return ret;
}

int addbillingProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[19];
    const unsigned char ln[6] = { 1, 0, 98, 1, 0, 255 };
    if ((ret = INIT_OBJECT(billingProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(billingProfile.captureObjects, CAPTURE_OBJECT, 19);
        //Set capture period to time in seconds.
        billingProfile.capturePeriod = 0;
        //Maximum row count.
        billingProfile.profileEntries = 7;
        billingProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(timeStampOfTheMostRecentBillingPeriodClosed);
        CAPTURE_OBJECT[0].attributeIndex = 2;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(cumulativeEnergyKwh);
        CAPTURE_OBJECT[1].attributeIndex = 2;
        CAPTURE_OBJECT[1].dataIndex = 0;
        CAPTURE_OBJECT[2].target = BASE(cumulativeEnergyKwhExport);
        CAPTURE_OBJECT[2].attributeIndex = 2;
        CAPTURE_OBJECT[2].dataIndex = 0;
        CAPTURE_OBJECT[3].target = BASE(cumulativeEnergyKvahImport);
        CAPTURE_OBJECT[3].attributeIndex = 2;
        CAPTURE_OBJECT[3].dataIndex = 0;
        CAPTURE_OBJECT[4].target = BASE(cumulativeEnergyKvahExport);
        CAPTURE_OBJECT[4].attributeIndex = 2;
        CAPTURE_OBJECT[4].dataIndex = 0;
        CAPTURE_OBJECT[5].target = BASE(sumLiReactivePowerQiTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[5].attributeIndex = 2;
        CAPTURE_OBJECT[5].dataIndex = 0;
        CAPTURE_OBJECT[6].target = BASE(sumLiReactivePowerQiiTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[6].attributeIndex = 2;
        CAPTURE_OBJECT[6].dataIndex = 0;
        CAPTURE_OBJECT[7].target = BASE(sumLiReactivePowerQiiiTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[7].attributeIndex = 2;
        CAPTURE_OBJECT[7].dataIndex = 0;
        CAPTURE_OBJECT[8].target = BASE(sumLiReactivePowerQivTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[8].attributeIndex = 2;
        CAPTURE_OBJECT[8].dataIndex = 0;
        CAPTURE_OBJECT[9].target = BASE(maximumDemandKw);
        CAPTURE_OBJECT[9].attributeIndex = 2;
        CAPTURE_OBJECT[9].dataIndex = 0;
        CAPTURE_OBJECT[10].target = BASE(maximumDemandKw);
        CAPTURE_OBJECT[10].attributeIndex = 5;
        CAPTURE_OBJECT[10].dataIndex = 0;
        CAPTURE_OBJECT[11].target = BASE(sumLiActivePowerMinusMax1RateIsTotal);
        CAPTURE_OBJECT[11].attributeIndex = 2;
        CAPTURE_OBJECT[11].dataIndex = 0;
        CAPTURE_OBJECT[12].target = BASE(sumLiActivePowerMinusMax1RateIsTotal);
        CAPTURE_OBJECT[12].attributeIndex = 5;
        CAPTURE_OBJECT[12].dataIndex = 0;
        CAPTURE_OBJECT[13].target = BASE(maximumDemandKva);
        CAPTURE_OBJECT[13].attributeIndex = 2;
        CAPTURE_OBJECT[13].dataIndex = 0;
        CAPTURE_OBJECT[14].target = BASE(maximumDemandKva);
        CAPTURE_OBJECT[14].attributeIndex = 5;
        CAPTURE_OBJECT[14].dataIndex = 0;
        CAPTURE_OBJECT[15].target = BASE(sumLiApparentPowerMinusMax1RateIsTotal);
        CAPTURE_OBJECT[15].attributeIndex = 2;
        CAPTURE_OBJECT[15].dataIndex = 0;
        CAPTURE_OBJECT[16].target = BASE(sumLiApparentPowerMinusMax1RateIsTotal);
        CAPTURE_OBJECT[16].attributeIndex = 5;
        CAPTURE_OBJECT[16].dataIndex = 0;
        CAPTURE_OBJECT[17].target = BASE(sumLiActivePowerTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[17].attributeIndex = 2;
        CAPTURE_OBJECT[17].dataIndex = 0;
        CAPTURE_OBJECT[18].target = BASE(identifiersForIndia);
        CAPTURE_OBJECT[18].attributeIndex = 3;
        CAPTURE_OBJECT[18].dataIndex = 0;

    }
    return ret;
}

int addloadProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[16];
    const unsigned char ln[6] = { 1, 0, 99, 1, 0, 255 };
    if ((ret = INIT_OBJECT(loadProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(loadProfile.captureObjects, CAPTURE_OBJECT, 16);
        //Set capture period to time in seconds.
        loadProfile.capturePeriod = 0;
        //Maximum row count.
        loadProfile.profileEntries = 2208;
        loadProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(clock1);
        CAPTURE_OBJECT[0].attributeIndex = 2;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(l1CurrentCurrentAvg5);
        CAPTURE_OBJECT[1].attributeIndex = 2;
        CAPTURE_OBJECT[1].dataIndex = 0;
        CAPTURE_OBJECT[2].target = BASE(l2CurrentCurrentAvg5);
        CAPTURE_OBJECT[2].attributeIndex = 2;
        CAPTURE_OBJECT[2].dataIndex = 0;
        CAPTURE_OBJECT[3].target = BASE(l3CurrentCurrentAvg5);
        CAPTURE_OBJECT[3].attributeIndex = 2;
        CAPTURE_OBJECT[3].dataIndex = 0;
        CAPTURE_OBJECT[4].target = BASE(l1VoltageCurrentAvg5);
        CAPTURE_OBJECT[4].attributeIndex = 2;
        CAPTURE_OBJECT[4].dataIndex = 0;
        CAPTURE_OBJECT[5].target = BASE(l2VoltageCurrentAvg5);
        CAPTURE_OBJECT[5].attributeIndex = 2;
        CAPTURE_OBJECT[5].dataIndex = 0;
        CAPTURE_OBJECT[6].target = BASE(l3VoltageCurrentAvg5);
        CAPTURE_OBJECT[6].attributeIndex = 2;
        CAPTURE_OBJECT[6].dataIndex = 0;
        CAPTURE_OBJECT[7].target = BASE(blockEnergyKwhImport);
        CAPTURE_OBJECT[7].attributeIndex = 2;
        CAPTURE_OBJECT[7].dataIndex = 0;
        CAPTURE_OBJECT[8].target = BASE(blockEnergyKwhExport);
        CAPTURE_OBJECT[8].attributeIndex = 2;
        CAPTURE_OBJECT[8].dataIndex = 0;
        CAPTURE_OBJECT[9].target = BASE(sumLiActivePowerTimeIntegral5);
        CAPTURE_OBJECT[9].attributeIndex = 2;
        CAPTURE_OBJECT[9].dataIndex = 0;
        CAPTURE_OBJECT[10].target = BASE(blockEnergyKvahImport);
        CAPTURE_OBJECT[10].attributeIndex = 2;
        CAPTURE_OBJECT[10].dataIndex = 0;
        CAPTURE_OBJECT[11].target = BASE(blockEnergyKvahExport);
        CAPTURE_OBJECT[11].attributeIndex = 2;
        CAPTURE_OBJECT[11].dataIndex = 0;
        CAPTURE_OBJECT[12].target = BASE(sumLiReactivePowerQiTimeIntegral5);
        CAPTURE_OBJECT[12].attributeIndex = 2;
        CAPTURE_OBJECT[12].dataIndex = 0;
        CAPTURE_OBJECT[13].target = BASE(sumLiReactivePowerQiiTimeIntegral5);
        CAPTURE_OBJECT[13].attributeIndex = 2;
        CAPTURE_OBJECT[13].dataIndex = 0;
        CAPTURE_OBJECT[14].target = BASE(sumLiReactivePowerQiiiTimeIntegral5);
        CAPTURE_OBJECT[14].attributeIndex = 2;
        CAPTURE_OBJECT[14].dataIndex = 0;
        CAPTURE_OBJECT[15].target = BASE(sumLiReactivePowerQivTimeIntegral5);
        CAPTURE_OBJECT[15].attributeIndex = 2;
        CAPTURE_OBJECT[15].dataIndex = 0;

    }
    return ret;
}

int adddailyLoadProfile()
{
    int ret;
    static gxTarget CAPTURE_OBJECT[10];
    const unsigned char ln[6] = { 1, 0, 99, 2, 0, 255 };
    if ((ret = INIT_OBJECT(dailyLoadProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        ARR_ATTACH(dailyLoadProfile.captureObjects, CAPTURE_OBJECT, 10);
        //Set capture period to time in seconds.
        dailyLoadProfile.capturePeriod = 0;
        //Maximum row count.
        dailyLoadProfile.profileEntries = 46;
        dailyLoadProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add colums.
        CAPTURE_OBJECT[0].target = BASE(clock1);
        CAPTURE_OBJECT[0].attributeIndex = 2;
        CAPTURE_OBJECT[0].dataIndex = 0;
        CAPTURE_OBJECT[1].target = BASE(cumulativeEnergyKwh);
        CAPTURE_OBJECT[1].attributeIndex = 2;
        CAPTURE_OBJECT[1].dataIndex = 0;
        CAPTURE_OBJECT[2].target = BASE(cumulativeEnergyKwhExport);
        CAPTURE_OBJECT[2].attributeIndex = 2;
        CAPTURE_OBJECT[2].dataIndex = 0;
        CAPTURE_OBJECT[3].target = BASE(cumulativeEnergyKvahImport);
        CAPTURE_OBJECT[3].attributeIndex = 2;
        CAPTURE_OBJECT[3].dataIndex = 0;
        CAPTURE_OBJECT[4].target = BASE(cumulativeEnergyKvahExport);
        CAPTURE_OBJECT[4].attributeIndex = 2;
        CAPTURE_OBJECT[4].dataIndex = 0;
        CAPTURE_OBJECT[5].target = BASE(sumLiReactivePowerQiTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[5].attributeIndex = 2;
        CAPTURE_OBJECT[5].dataIndex = 0;
        CAPTURE_OBJECT[6].target = BASE(sumLiReactivePowerQiiTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[6].attributeIndex = 2;
        CAPTURE_OBJECT[6].dataIndex = 0;
        CAPTURE_OBJECT[7].target = BASE(sumLiReactivePowerQiiiTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[7].attributeIndex = 2;
        CAPTURE_OBJECT[7].dataIndex = 0;
        CAPTURE_OBJECT[8].target = BASE(sumLiReactivePowerQivTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[8].attributeIndex = 2;
        CAPTURE_OBJECT[8].dataIndex = 0;
        CAPTURE_OBJECT[9].target = BASE(sumLiActivePowerTimeIntegral1RateIsTotal);
        CAPTURE_OBJECT[9].attributeIndex = 2;
        CAPTURE_OBJECT[9].dataIndex = 0;

    }
    return ret;
}

int addbillingDateImportMode()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 0, 1, 2, 255 };
    if ((ret = INIT_OBJECT(billingDateImportMode, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        static unsigned char data[12];


        GX_OCTECT_STRING(billingDateImportMode.value, data, 12);
        billingDateImportMode.scaler = 0;
        billingDateImportMode.unit = DLMS_UNIT_NO_UNIT;
    }
    return ret;
}

int addcumulativePowerOffDurationInMin()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 94, 91, 8, 255 };
    if ((ret = INIT_OBJECT(cumulativePowerOffDurationInMin, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT32(cumulativePowerOffDurationInMin.value) = 62346;
        cumulativePowerOffDurationInMin.scaler = 0;
        cumulativePowerOffDurationInMin.unit = DLMS_UNIT_MINUTE;
    }
    return ret;
}

int addactivePowerKw()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 1, 7, 0, 255 };
    if ((ret = INIT_OBJECT(activePowerKw, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_INT32(activePowerKw.value) = 0;
        activePowerKw.scaler = 0;
        activePowerKw.unit = DLMS_UNIT_ACTIVE_POWER;
    }
    return ret;
}

int addcumulativeEnergyKwh()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 1, 8, 0, 255 };
    if ((ret = INIT_OBJECT(cumulativeEnergyKwh, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT32(cumulativeEnergyKwh.value) = 1882;
        cumulativeEnergyKwh.scaler = 0;
        cumulativeEnergyKwh.unit = DLMS_UNIT_ACTIVE_ENERGY;
    }
    return ret;
}

int addcumulativeEnergyKwhExport()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 2, 8, 0, 255 };
    if ((ret = INIT_OBJECT(cumulativeEnergyKwhExport, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT32(cumulativeEnergyKwhExport.value) = 424;
        cumulativeEnergyKwhExport.scaler = 0;
        cumulativeEnergyKwhExport.unit = DLMS_UNIT_ACTIVE_ENERGY;
    }
    return ret;
}

int addsumLiReactivePowerPlusInstValue()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 3, 7, 0, 255 };
    if ((ret = INIT_OBJECT(sumLiReactivePowerPlusInstValue, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_INT32(sumLiReactivePowerPlusInstValue.value) = 0;
        sumLiReactivePowerPlusInstValue.scaler = 0;
        sumLiReactivePowerPlusInstValue.unit = DLMS_UNIT_REACTIVE_POWER;
    }
    return ret;
}

int addsumLiReactivePowerQiTimeIntegral1RateIsTotal()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 5, 8, 0, 255 };
    if ((ret = INIT_OBJECT(sumLiReactivePowerQiTimeIntegral1RateIsTotal, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT32(sumLiReactivePowerQiTimeIntegral1RateIsTotal.value) = 176;
        sumLiReactivePowerQiTimeIntegral1RateIsTotal.scaler = 0;
        sumLiReactivePowerQiTimeIntegral1RateIsTotal.unit = DLMS_UNIT_REACTIVE_ENERGY;
    }
    return ret;
}

int addsumLiReactivePowerQiiTimeIntegral1RateIsTotal()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 6, 8, 0, 255 };
    if ((ret = INIT_OBJECT(sumLiReactivePowerQiiTimeIntegral1RateIsTotal, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT32(sumLiReactivePowerQiiTimeIntegral1RateIsTotal.value) = 150;
        sumLiReactivePowerQiiTimeIntegral1RateIsTotal.scaler = 0;
        sumLiReactivePowerQiiTimeIntegral1RateIsTotal.unit = DLMS_UNIT_REACTIVE_ENERGY;
    }
    return ret;
}

int addsumLiReactivePowerQiiiTimeIntegral1RateIsTotal()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 7, 8, 0, 255 };
    if ((ret = INIT_OBJECT(sumLiReactivePowerQiiiTimeIntegral1RateIsTotal, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT32(sumLiReactivePowerQiiiTimeIntegral1RateIsTotal.value) = 152;
        sumLiReactivePowerQiiiTimeIntegral1RateIsTotal.scaler = 0;
        sumLiReactivePowerQiiiTimeIntegral1RateIsTotal.unit = DLMS_UNIT_REACTIVE_ENERGY;
    }
    return ret;
}

int addsumLiReactivePowerQivTimeIntegral1RateIsTotal()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 8, 8, 0, 255 };
    if ((ret = INIT_OBJECT(sumLiReactivePowerQivTimeIntegral1RateIsTotal, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT32(sumLiReactivePowerQivTimeIntegral1RateIsTotal.value) = 178;
        sumLiReactivePowerQivTimeIntegral1RateIsTotal.scaler = 0;
        sumLiReactivePowerQivTimeIntegral1RateIsTotal.unit = DLMS_UNIT_REACTIVE_ENERGY;
    }
    return ret;
}

int addapparentPowerKva()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 9, 7, 0, 255 };
    if ((ret = INIT_OBJECT(apparentPowerKva, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT32(apparentPowerKva.value) = 0;
        apparentPowerKva.scaler = 0;
        apparentPowerKva.unit = DLMS_UNIT_APPARENT_POWER;
    }
    return ret;
}

int addcumulativeEnergyKvahImport()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 9, 8, 0, 255 };
    if ((ret = INIT_OBJECT(cumulativeEnergyKvahImport, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT32(cumulativeEnergyKvahImport.value) = 1985;
        cumulativeEnergyKvahImport.scaler = 0;
        cumulativeEnergyKvahImport.unit = DLMS_UNIT_APPARENT_ENERGY;
    }
    return ret;
}

int addcumulativeEnergyKvahExport()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 10, 8, 0, 255 };
    if ((ret = INIT_OBJECT(cumulativeEnergyKvahExport, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT32(cumulativeEnergyKvahExport.value) = 511;
        cumulativeEnergyKvahExport.scaler = 0;
        cumulativeEnergyKvahExport.unit = DLMS_UNIT_APPARENT_ENERGY;
    }
    return ret;
}

int addsignedPowerFactor()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 13, 7, 0, 255 };
    if ((ret = INIT_OBJECT(signedPowerFactor, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_INT8(signedPowerFactor.value) = 100;
        signedPowerFactor.scaler = -2;
        signedPowerFactor.unit = DLMS_UNIT_NO_UNIT;
    }
    return ret;
}

int addfrequencyHz()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 14, 7, 0, 255 };
    if ((ret = INIT_OBJECT(frequencyHz, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT16(frequencyHz.value) = 5006;
        frequencyHz.scaler = -2;
        frequencyHz.unit = DLMS_UNIT_FREQUENCY;
    }
    return ret;
}

int addl1CurrentInstValue()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 31, 7, 0, 255 };
    if ((ret = INIT_OBJECT(l1CurrentInstValue, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT16(l1CurrentInstValue.value) = 0;
        l1CurrentInstValue.scaler = -2;
        l1CurrentInstValue.unit = DLMS_UNIT_CURRENT;
    }
    return ret;
}

int addl1VoltageInstValue()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 32, 7, 0, 255 };
    if ((ret = INIT_OBJECT(l1VoltageInstValue, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT16(l1VoltageInstValue.value) = 22980;
        l1VoltageInstValue.scaler = -2;
        l1VoltageInstValue.unit = DLMS_UNIT_VOLTAGE;
    }
    return ret;
}

int addl1PowerFactorInstValue()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 33, 7, 0, 255 };
    if ((ret = INIT_OBJECT(l1PowerFactorInstValue, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_INT8(l1PowerFactorInstValue.value) = 100;
        l1PowerFactorInstValue.scaler = -2;
        l1PowerFactorInstValue.unit = DLMS_UNIT_NO_UNIT;
    }
    return ret;
}

int addl2CurrentInstValue()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 51, 7, 0, 255 };
    if ((ret = INIT_OBJECT(l2CurrentInstValue, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT16(l2CurrentInstValue.value) = 0;
        l2CurrentInstValue.scaler = -2;
        l2CurrentInstValue.unit = DLMS_UNIT_CURRENT;
    }
    return ret;
}

int addl2VoltageInstValue()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 52, 7, 0, 255 };
    if ((ret = INIT_OBJECT(l2VoltageInstValue, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT16(l2VoltageInstValue.value) = 22978;
        l2VoltageInstValue.scaler = -2;
        l2VoltageInstValue.unit = DLMS_UNIT_VOLTAGE;
    }
    return ret;
}

int addl2PowerFactorInstValue()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 53, 7, 0, 255 };
    if ((ret = INIT_OBJECT(l2PowerFactorInstValue, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_INT8(l2PowerFactorInstValue.value) = 100;
        l2PowerFactorInstValue.scaler = -2;
        l2PowerFactorInstValue.unit = DLMS_UNIT_NO_UNIT;
    }
    return ret;
}

int addl3CurrentInstValue()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 71, 7, 0, 255 };
    if ((ret = INIT_OBJECT(l3CurrentInstValue, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT16(l3CurrentInstValue.value) = 0;
        l3CurrentInstValue.scaler = -2;
        l3CurrentInstValue.unit = DLMS_UNIT_CURRENT;
    }
    return ret;
}

int addl3VoltageInstValue()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 72, 7, 0, 255 };
    if ((ret = INIT_OBJECT(l3VoltageInstValue, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT16(l3VoltageInstValue.value) = 23005;
        l3VoltageInstValue.scaler = -2;
        l3VoltageInstValue.unit = DLMS_UNIT_VOLTAGE;
    }
    return ret;
}

int addl3PowerFactorInstValue()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 73, 7, 0, 255 };
    if ((ret = INIT_OBJECT(l3PowerFactorInstValue, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_INT8(l3PowerFactorInstValue.value) = 100;
        l3PowerFactorInstValue.scaler = -2;
        l3PowerFactorInstValue.unit = DLMS_UNIT_NO_UNIT;
    }
    return ret;
}

int addsumLiActivePowerTimeIntegral1RateIsTotal()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 16, 8, 0, 255 };
    if ((ret = INIT_OBJECT(sumLiActivePowerTimeIntegral1RateIsTotal, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        sumLiActivePowerTimeIntegral1RateIsTotal.scaler = 0;
        sumLiActivePowerTimeIntegral1RateIsTotal.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addl1CurrentCurrentAvg5()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 31, 27, 0, 255 };
    if ((ret = INIT_OBJECT(l1CurrentCurrentAvg5, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        l1CurrentCurrentAvg5.scaler = 0;
        l1CurrentCurrentAvg5.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addl2CurrentCurrentAvg5()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 51, 27, 0, 255 };
    if ((ret = INIT_OBJECT(l2CurrentCurrentAvg5, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        l2CurrentCurrentAvg5.scaler = 0;
        l2CurrentCurrentAvg5.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addl3CurrentCurrentAvg5()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 71, 27, 0, 255 };
    if ((ret = INIT_OBJECT(l3CurrentCurrentAvg5, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        l3CurrentCurrentAvg5.scaler = 0;
        l3CurrentCurrentAvg5.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addl1VoltageCurrentAvg5()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 32, 27, 0, 255 };
    if ((ret = INIT_OBJECT(l1VoltageCurrentAvg5, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        l1VoltageCurrentAvg5.scaler = 0;
        l1VoltageCurrentAvg5.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addl2VoltageCurrentAvg5()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 52, 27, 0, 255 };
    if ((ret = INIT_OBJECT(l2VoltageCurrentAvg5, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        l2VoltageCurrentAvg5.scaler = 0;
        l2VoltageCurrentAvg5.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addl3VoltageCurrentAvg5()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 72, 27, 0, 255 };
    if ((ret = INIT_OBJECT(l3VoltageCurrentAvg5, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        l3VoltageCurrentAvg5.scaler = 0;
        l3VoltageCurrentAvg5.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addblockEnergyKwhImport()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 1, 29, 0, 255 };
    if ((ret = INIT_OBJECT(blockEnergyKwhImport, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        blockEnergyKwhImport.scaler = 0;
        blockEnergyKwhImport.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addblockEnergyKwhExport()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 2, 29, 0, 255 };
    if ((ret = INIT_OBJECT(blockEnergyKwhExport, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        blockEnergyKwhExport.scaler = 0;
        blockEnergyKwhExport.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addsumLiActivePowerTimeIntegral5()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 16, 29, 0, 255 };
    if ((ret = INIT_OBJECT(sumLiActivePowerTimeIntegral5, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        sumLiActivePowerTimeIntegral5.scaler = 0;
        sumLiActivePowerTimeIntegral5.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addblockEnergyKvahImport()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 9, 29, 0, 255 };
    if ((ret = INIT_OBJECT(blockEnergyKvahImport, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        blockEnergyKvahImport.scaler = 0;
        blockEnergyKvahImport.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addblockEnergyKvahExport()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 10, 29, 0, 255 };
    if ((ret = INIT_OBJECT(blockEnergyKvahExport, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        blockEnergyKvahExport.scaler = 0;
        blockEnergyKvahExport.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addsumLiReactivePowerQiTimeIntegral5()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 5, 29, 0, 255 };
    if ((ret = INIT_OBJECT(sumLiReactivePowerQiTimeIntegral5, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        sumLiReactivePowerQiTimeIntegral5.scaler = 0;
        sumLiReactivePowerQiTimeIntegral5.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addsumLiReactivePowerQiiTimeIntegral5()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 6, 29, 0, 255 };
    if ((ret = INIT_OBJECT(sumLiReactivePowerQiiTimeIntegral5, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        sumLiReactivePowerQiiTimeIntegral5.scaler = 0;
        sumLiReactivePowerQiiTimeIntegral5.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addsumLiReactivePowerQiiiTimeIntegral5()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 7, 29, 0, 255 };
    if ((ret = INIT_OBJECT(sumLiReactivePowerQiiiTimeIntegral5, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        sumLiReactivePowerQiiiTimeIntegral5.scaler = 0;
        sumLiReactivePowerQiiiTimeIntegral5.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addsumLiReactivePowerQivTimeIntegral5()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 8, 29, 0, 255 };
    if ((ret = INIT_OBJECT(sumLiReactivePowerQivTimeIntegral5, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        sumLiReactivePowerQivTimeIntegral5.scaler = 0;
        sumLiReactivePowerQivTimeIntegral5.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int addidentifiersForIndia()
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 94, 91, 13, 255 };
    if ((ret = INIT_OBJECT(identifiersForIndia, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {

        identifiersForIndia.scaler = 0;
        identifiersForIndia.unit = DLMS_UNIT_NONE;
    }
    return ret;
}

int obj_InitObjects(dlmsServerSettings* settings)
{
    int ret;
    if ((ret = addllsSecretKey()) != 0 ||
        (ret = addclock1()) != 0 ||
        (ret = addcumulativeBillingCount()) != 0 ||
        (ret = addcosemLogicalDeviceName()) != 0 ||
        (ret = addcumulativeTamperCount()) != 0 ||
        (ret = addmeterType()) != 0 ||
        (ret = addmeterSerialNumber()) != 0 ||
        (ret = addmanufacturerName()) != 0 ||
        (ret = addmeterYearOfManufacture()) != 0 ||
        (ret = addcumulativeProgrammingCount()) != 0 ||
        (ret = addnoOfPowerFailuresInAllThreePhases()) != 0 ||
        (ret = addeventCode1()) != 0 ||
        (ret = addeventCurrentRelated()) != 0 ||
        (ret = addeventPowerRelated()) != 0 ||
        (ret = addeventTransactionRelated()) != 0 ||
        (ret = addeventOthers()) != 0 ||
        (ret = addeventNonRollOverEvents()) != 0 ||
        (ret = addeventLoadSwitchStatus()) != 0 ||
        (ret = addfirmwareVersionForMeter()) != 0 ||
        (ret = addtransformerRatioMinusCurrent()) != 0 ||
        (ret = addtransformerRatioMinusVoltage()) != 0 ||
        (ret = adddemandIntegrationPeriod()) != 0 ||
        (ret = addprofileCapturePeriod()) != 0 ||
        (ret = addrecordingInterval2ForLoadProfile()) != 0 ||
        (ret = addtimeStampOfTheMostRecentBillingPeriodClosed()) != 0 ||
        (ret = addmaximumDemandKw()) != 0 ||
        (ret = addsumLiActivePowerMinusMax1RateIsTotal()) != 0 ||
        (ret = addmaximumDemandKva()) != 0 ||
        (ret = addsumLiApparentPowerMinusMax1RateIsTotal()) != 0 ||
        (ret = addiecHdlcSetup()) != 0 ||
        (ret = addvoltageRelatedEventsProfile()) != 0 ||
        (ret = addcurrentRelatedEventsProfile()) != 0 ||
        (ret = addpowerRelatedEventsProfile()) != 0 ||
        (ret = addtransactionEventsProfile()) != 0 ||
        (ret = addotherTamperEventsProfile()) != 0 ||
        (ret = addnonRollOverEventsProfile()) != 0 ||
        (ret = addcontrolEventsProfile()) != 0 ||
        (ret = addinstantaneousProfile()) != 0 ||
        (ret = addscalerInstantaneousProfile()) != 0 ||
        (ret = addscalerBlockLoadProfile()) != 0 ||
        (ret = addscalerDailyLoadProfile()) != 0 ||
        (ret = addscalerBillingProfile()) != 0 ||
        (ret = addscalerEventsProfile()) != 0 ||
        (ret = addbillingProfile()) != 0 ||
        (ret = addloadProfile()) != 0 ||
        (ret = adddailyLoadProfile()) != 0 ||
        (ret = addbillingDateImportMode()) != 0 ||
        (ret = addcumulativePowerOffDurationInMin()) != 0 ||
        (ret = addactivePowerKw()) != 0 ||
        (ret = addcumulativeEnergyKwh()) != 0 ||
        (ret = addcumulativeEnergyKwhExport()) != 0 ||
        (ret = addsumLiReactivePowerPlusInstValue()) != 0 ||
        (ret = addsumLiReactivePowerQiTimeIntegral1RateIsTotal()) != 0 ||
        (ret = addsumLiReactivePowerQiiTimeIntegral1RateIsTotal()) != 0 ||
        (ret = addsumLiReactivePowerQiiiTimeIntegral1RateIsTotal()) != 0 ||
        (ret = addsumLiReactivePowerQivTimeIntegral1RateIsTotal()) != 0 ||
        (ret = addapparentPowerKva()) != 0 ||
        (ret = addcumulativeEnergyKvahImport()) != 0 ||
        (ret = addcumulativeEnergyKvahExport()) != 0 ||
        (ret = addsignedPowerFactor()) != 0 ||
        (ret = addfrequencyHz()) != 0 ||
        (ret = addl1CurrentInstValue()) != 0 ||
        (ret = addl1VoltageInstValue()) != 0 ||
        (ret = addl1PowerFactorInstValue()) != 0 ||
        (ret = addl2CurrentInstValue()) != 0 ||
        (ret = addl2VoltageInstValue()) != 0 ||
        (ret = addl2PowerFactorInstValue()) != 0 ||
        (ret = addl3CurrentInstValue()) != 0 ||
        (ret = addl3VoltageInstValue()) != 0 ||
        (ret = addl3PowerFactorInstValue()) != 0 ||
        (ret = addsumLiActivePowerTimeIntegral1RateIsTotal()) != 0 ||
        (ret = addl1CurrentCurrentAvg5()) != 0 ||
        (ret = addl2CurrentCurrentAvg5()) != 0 ||
        (ret = addl3CurrentCurrentAvg5()) != 0 ||
        (ret = addl1VoltageCurrentAvg5()) != 0 ||
        (ret = addl2VoltageCurrentAvg5()) != 0 ||
        (ret = addl3VoltageCurrentAvg5()) != 0 ||
        (ret = addblockEnergyKwhImport()) != 0 ||
        (ret = addblockEnergyKwhExport()) != 0 ||
        (ret = addsumLiActivePowerTimeIntegral5()) != 0 ||
        (ret = addblockEnergyKvahImport()) != 0 ||
        (ret = addblockEnergyKvahExport()) != 0 ||
        (ret = addsumLiReactivePowerQiTimeIntegral5()) != 0 ||
        (ret = addsumLiReactivePowerQiiTimeIntegral5()) != 0 ||
        (ret = addsumLiReactivePowerQiiiTimeIntegral5()) != 0 ||
        (ret = addsumLiReactivePowerQivTimeIntegral5()) != 0 ||
        (ret = addidentifiersForIndia()) != 0)
    {
        return ret;
    }
    return 0;
}
