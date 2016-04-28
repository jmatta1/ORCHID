/*
 * Note: this file originally auto-generated by mib2c using
 *        $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "crate.h"

/** Initialize the outputTable table by defining its contents and how it's structured */
void
initialize_table_outputTable(void)
{
    const oid outputTable_oid[] = {1,3,6,1,4,1,19947,1,3,2};
    netsnmp_table_data_set *table_set;

    /* create the table structure itself */
    table_set = netsnmp_create_table_data_set("outputTable");

    /* comment this out or delete if you don't support creation of new rows */
    table_set->allow_creation = 1;

    /***************************************************
     * Adding indexes
     */
    DEBUGMSGTL(("initialize_table_outputTable",
                "adding indexes to table outputTable\n"));
    netsnmp_table_set_add_indexes(table_set,
                           ASN_INTEGER,  /* index: outputIndex */
                           0);

    DEBUGMSGTL(("initialize_table_outputTable",
                "adding column types to table outputTable\n"));		 
    netsnmp_table_set_multi_add_default_row(table_set,
                                            COLUMN_OUTPUTINDEX, ASN_INTEGER, 0,
                                            NULL, 0,
                                            COLUMN_OUTPUTNAME, ASN_OCTET_STR, 0,
                                            NULL, 0,
                                            COLUMN_OUTPUTGROUP, ASN_INTEGER, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTSTATUS, ASN_OCTET_STR, 0,
                                            NULL, 0,
                                            COLUMN_OUTPUTMEASUREMENTSENSEVOLTAGE, ASN_OPAQUE, 0,
                                            NULL, 0,
                                            COLUMN_OUTPUTMEASUREMENTTERMINALVOLTAGE, ASN_OPAQUE, 0,
                                            NULL, 0,
                                            COLUMN_OUTPUTMEASUREMENTCURRENT, ASN_OPAQUE, 0,
                                            NULL, 0,
                                            COLUMN_OUTPUTMEASUREMENTTEMPERATURE, ASN_INTEGER, 0,
                                            NULL, 0,
                                            COLUMN_OUTPUTSWITCH, ASN_INTEGER, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTVOLTAGE, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTADJUSTVOLTAGE, ASN_INTEGER, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTCURRENT, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTVOLTAGERISERATE, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTVOLTAGEFALLRATE, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTSUPERVISIONBEHAVIOR, ASN_INTEGER, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTSUPERVISIONMINSENSEVOLTAGE, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTSUPERVISIONMAXSENSEVOLTAGE, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTSUPERVISIONMAXTERMINALVOLTAGE, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTSUPERVISIONMAXCURRENT, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTSUPERVISIONMAXTEMPERATURE, ASN_INTEGER, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTCONFIGMAXSENSEVOLTAGE, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTCONFIGMAXTERMINALVOLTAGE, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTCONFIGMAXCURRENT, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTSUPERVISIONMAXPOWER, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTCURRENTRISERATE, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTCURRENTFALLRATE, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTTRIPTIMEMAXCURRENT, ASN_INTEGER, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTHARDWARELIMITVOLTAGE, ASN_OPAQUE, 0,
                                            NULL, 0,
                                            COLUMN_OUTPUTHARDWARELIMITCURRENT, ASN_OPAQUE, 0,
                                            NULL, 0,
                                            COLUMN_OUTPUTCONFIGGAINSENSEVOLTAGE, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTCONFIGOFFSETSENSEVOLTAGE, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTCONFIGGAINTERMINALVOLTAGE, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTCONFIGOFFSETTERMINALVOLTAGE, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTCONFIGGAINCURRENT, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTCONFIGOFFSETCURRENT, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTUSERCONFIG, ASN_INTEGER, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTREGULATIONMODE, ASN_INTEGER, 1,
                                            NULL, 0,
                                            COLUMN_OUTPUTCONFIGMAXTEMPERATURE, ASN_INTEGER, 1,
                                            NULL, 0,
                              0);
    
    /* registering the table with the master agent */
    /* note: if you don't need a subhandler to deal with any aspects
       of the request, change outputTable_handler to "NULL" */
    netsnmp_register_table_data_set(netsnmp_create_handler_registration("outputTable", outputTable_handler,
                                                        outputTable_oid,
                                                        OID_LENGTH(outputTable_oid),
                                                        HANDLER_CAN_RWRITE),
                            table_set, NULL);
}
/** Initialize the groupsTable table by defining its contents and how it's structured */
void
initialize_table_groupsTable(void)
{
    const oid groupsTable_oid[] = {1,3,6,1,4,1,19947,1,3,4};
    netsnmp_table_data_set *table_set;

    /* create the table structure itself */
    table_set = netsnmp_create_table_data_set("groupsTable");

    /* comment this out or delete if you don't support creation of new rows */
    table_set->allow_creation = 1;

    /***************************************************
     * Adding indexes
     */
    DEBUGMSGTL(("initialize_table_groupsTable",
                "adding indexes to table groupsTable\n"));
    netsnmp_table_set_add_indexes(table_set,
                           ASN_INTEGER,  /* index: groupsIndex */
                           0);

    DEBUGMSGTL(("initialize_table_groupsTable",
                "adding column types to table groupsTable\n"));		 
    netsnmp_table_set_multi_add_default_row(table_set,
                                            COLUMN_GROUPSINDEX, ASN_INTEGER, 0,
                                            NULL, 0,
                                            COLUMN_GROUPSSWITCH, ASN_INTEGER, 1,
                                            NULL, 0,
                              0);
    
    /* registering the table with the master agent */
    /* note: if you don't need a subhandler to deal with any aspects
       of the request, change groupsTable_handler to "NULL" */
    netsnmp_register_table_data_set(netsnmp_create_handler_registration("groupsTable", groupsTable_handler,
                                                        groupsTable_oid,
                                                        OID_LENGTH(groupsTable_oid),
                                                        HANDLER_CAN_RWRITE),
                            table_set, NULL);
}
/** Initialize the moduleTable table by defining its contents and how it's structured */
void
initialize_table_moduleTable(void)
{
    const oid moduleTable_oid[] = {1,3,6,1,4,1,19947,1,3,6};
    netsnmp_table_data_set *table_set;

    /* create the table structure itself */
    table_set = netsnmp_create_table_data_set("moduleTable");

    /* comment this out or delete if you don't support creation of new rows */
    table_set->allow_creation = 1;

    /***************************************************
     * Adding indexes
     */
    DEBUGMSGTL(("initialize_table_moduleTable",
                "adding indexes to table moduleTable\n"));
    netsnmp_table_set_add_indexes(table_set,
                           ASN_INTEGER,  /* index: moduleIndex */
                           0);

    DEBUGMSGTL(("initialize_table_moduleTable",
                "adding column types to table moduleTable\n"));		 
    netsnmp_table_set_multi_add_default_row(table_set,
                                            COLUMN_MODULEINDEX, ASN_INTEGER, 0,
                                            NULL, 0,
                                            COLUMN_MODULEDESCRIPTION, ASN_OCTET_STR, 0,
                                            NULL, 0,
                                            COLUMN_MODULEAUXILIARYMEASUREMENTVOLTAGE, , 0,
                                            NULL, 0,
                                            COLUMN_MODULEHARDWARELIMITVOLTAGE, ASN_OPAQUE, 0,
                                            NULL, 0,
                                            COLUMN_MODULEHARDWARELIMITCURRENT, ASN_OPAQUE, 0,
                                            NULL, 0,
                                            COLUMN_MODULERAMPSPEEDVOLTAGE, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_MODULERAMPSPEEDCURRENT, ASN_OPAQUE, 1,
                                            NULL, 0,
                                            COLUMN_MODULESTATUS, ASN_OCTET_STR, 0,
                                            NULL, 0,
                                            COLUMN_MODULEEVENTSTATUS, ASN_OCTET_STR, 0,
                                            NULL, 0,
                                            COLUMN_MODULEEVENTCHANNELSTATUS, ASN_INTEGER, 0,
                                            NULL, 0,
                                            COLUMN_MODULEDOCLEAR, ASN_INTEGER, 1,
                                            NULL, 0,
                                            COLUMN_MODULEAUXILIARYMEASUREMENTTEMPERATURE, , 0,
                                            NULL, 0,
                              0);
    
    /* registering the table with the master agent */
    /* note: if you don't need a subhandler to deal with any aspects
       of the request, change moduleTable_handler to "NULL" */
    netsnmp_register_table_data_set(netsnmp_create_handler_registration("moduleTable", moduleTable_handler,
                                                        moduleTable_oid,
                                                        OID_LENGTH(moduleTable_oid),
                                                        HANDLER_CAN_RWRITE),
                            table_set, NULL);
}
/** Initialize the sensorTable table by defining its contents and how it's structured */
void
initialize_table_sensorTable(void)
{
    const oid sensorTable_oid[] = {1,3,6,1,4,1,19947,1,4,2};
    netsnmp_table_data_set *table_set;

    /* create the table structure itself */
    table_set = netsnmp_create_table_data_set("sensorTable");

    /* comment this out or delete if you don't support creation of new rows */
    table_set->allow_creation = 1;

    /***************************************************
     * Adding indexes
     */
    DEBUGMSGTL(("initialize_table_sensorTable",
                "adding indexes to table sensorTable\n"));
    netsnmp_table_set_add_indexes(table_set,
                           ASN_INTEGER,  /* index: sensorIndex */
                           0);

    DEBUGMSGTL(("initialize_table_sensorTable",
                "adding column types to table sensorTable\n"));		 
    netsnmp_table_set_multi_add_default_row(table_set,
                                            COLUMN_SENSORINDEX, ASN_INTEGER, 0,
                                            NULL, 0,
                                            COLUMN_SENSORTEMPERATURE, ASN_INTEGER, 0,
                                            NULL, 0,
                                            COLUMN_SENSORWARNINGTHRESHOLD, ASN_INTEGER, 1,
                                            NULL, 0,
                                            COLUMN_SENSORFAILURETHRESHOLD, ASN_INTEGER, 1,
                                            NULL, 0,
                                            COLUMN_SENSORALARMTHRESHOLD, ASN_INTEGER, 1,
                                            NULL, 0,
                                            COLUMN_SENSORNAME, ASN_OCTET_STR, 1,
                                            NULL, 0,
                                            COLUMN_SENSORID, ASN_OCTET_STR, 1,
                                            NULL, 0,
                                            COLUMN_SENSORSTATUS, ASN_INTEGER, 0,
                                            NULL, 0,
                              0);
    
    /* registering the table with the master agent */
    /* note: if you don't need a subhandler to deal with any aspects
       of the request, change sensorTable_handler to "NULL" */
    netsnmp_register_table_data_set(netsnmp_create_handler_registration("sensorTable", sensorTable_handler,
                                                        sensorTable_oid,
                                                        OID_LENGTH(sensorTable_oid),
                                                        HANDLER_CAN_RWRITE),
                            table_set, NULL);
}
/** Initialize the snmpCommunityTable table by defining its contents and how it's structured */
void
initialize_table_snmpCommunityTable(void)
{
    const oid snmpCommunityTable_oid[] = {1,3,6,1,4,1,19947,1,5,1,1};
    netsnmp_table_data_set *table_set;

    /* create the table structure itself */
    table_set = netsnmp_create_table_data_set("snmpCommunityTable");

    /* comment this out or delete if you don't support creation of new rows */
    table_set->allow_creation = 1;

    /***************************************************
     * Adding indexes
     */
    DEBUGMSGTL(("initialize_table_snmpCommunityTable",
                "adding indexes to table snmpCommunityTable\n"));
    netsnmp_table_set_add_indexes(table_set,
                           ASN_INTEGER,  /* index: snmpAccessRight */
                           0);

    DEBUGMSGTL(("initialize_table_snmpCommunityTable",
                "adding column types to table snmpCommunityTable\n"));		 
    netsnmp_table_set_multi_add_default_row(table_set,
                                            COLUMN_SNMPACCESSRIGHT, ASN_INTEGER, 0,
                                            NULL, 0,
                                            COLUMN_SNMPCOMMUNITYNAME, ASN_OCTET_STR, 1,
                                            NULL, 0,
                              0);
    
    /* registering the table with the master agent */
    /* note: if you don't need a subhandler to deal with any aspects
       of the request, change snmpCommunityTable_handler to "NULL" */
    netsnmp_register_table_data_set(netsnmp_create_handler_registration("snmpCommunityTable", snmpCommunityTable_handler,
                                                        snmpCommunityTable_oid,
                                                        OID_LENGTH(snmpCommunityTable_oid),
                                                        HANDLER_CAN_RWRITE),
                            table_set, NULL);
}
/** Initialize the psAuxiliaryTable table by defining its contents and how it's structured */
void
initialize_table_psAuxiliaryTable(void)
{
    const oid psAuxiliaryTable_oid[] = {1,3,6,1,4,1,19947,1,6,5};
    netsnmp_table_data_set *table_set;

    /* create the table structure itself */
    table_set = netsnmp_create_table_data_set("psAuxiliaryTable");

    /* comment this out or delete if you don't support creation of new rows */
    table_set->allow_creation = 1;

    /***************************************************
     * Adding indexes
     */
    DEBUGMSGTL(("initialize_table_psAuxiliaryTable",
                "adding indexes to table psAuxiliaryTable\n"));
    netsnmp_table_set_add_indexes(table_set,
                           ASN_INTEGER,  /* index: psAuxiliaryIndex */
                           0);

    DEBUGMSGTL(("initialize_table_psAuxiliaryTable",
                "adding column types to table psAuxiliaryTable\n"));		 
    netsnmp_table_set_multi_add_default_row(table_set,
                                            COLUMN_PSAUXILIARYINDEX, ASN_INTEGER, 0,
                                            NULL, 0,
                                            COLUMN_PSAUXILIARYMEASUREMENTVOLTAGE, ASN_OPAQUE, 0,
                                            NULL, 0,
                                            COLUMN_PSAUXILIARYMEASUREMENTCURRENT, ASN_OPAQUE, 0,
                                            NULL, 0,
                              0);
    
    /* registering the table with the master agent */
    /* note: if you don't need a subhandler to deal with any aspects
       of the request, change psAuxiliaryTable_handler to "NULL" */
    netsnmp_register_table_data_set(netsnmp_create_handler_registration("psAuxiliaryTable", psAuxiliaryTable_handler,
                                                        psAuxiliaryTable_oid,
                                                        OID_LENGTH(psAuxiliaryTable_oid),
                                                        HANDLER_CAN_RWRITE),
                            table_set, NULL);
}
/** Initialize the fanSpeedTable table by defining its contents and how it's structured */
void
initialize_table_fanSpeedTable(void)
{
    const oid fanSpeedTable_oid[] = {1,3,6,1,4,1,19947,1,7,8};
    netsnmp_table_data_set *table_set;

    /* create the table structure itself */
    table_set = netsnmp_create_table_data_set("fanSpeedTable");

    /* comment this out or delete if you don't support creation of new rows */
    table_set->allow_creation = 1;

    /***************************************************
     * Adding indexes
     */
    DEBUGMSGTL(("initialize_table_fanSpeedTable",
                "adding indexes to table fanSpeedTable\n"));
    netsnmp_table_set_add_indexes(table_set,
                           ASN_INTEGER,  /* index: fanNumber */
                           0);

    DEBUGMSGTL(("initialize_table_fanSpeedTable",
                "adding column types to table fanSpeedTable\n"));		 
    netsnmp_table_set_multi_add_default_row(table_set,
                                            COLUMN_FANNUMBER, ASN_INTEGER, 0,
                                            NULL, 0,
                                            COLUMN_FANSPEED, ASN_INTEGER, 0,
                                            NULL, 0,
                              0);
    
    /* registering the table with the master agent */
    /* note: if you don't need a subhandler to deal with any aspects
       of the request, change fanSpeedTable_handler to "NULL" */
    netsnmp_register_table_data_set(netsnmp_create_handler_registration("fanSpeedTable", fanSpeedTable_handler,
                                                        fanSpeedTable_oid,
                                                        OID_LENGTH(fanSpeedTable_oid),
                                                        HANDLER_CAN_RWRITE),
                            table_set, NULL);
}
/** Initialize the analogInputTable table by defining its contents and how it's structured */
void
initialize_table_analogInputTable(void)
{
    const oid analogInputTable_oid[] = {1,3,6,1,4,1,19947,1,9,2};
    netsnmp_table_data_set *table_set;

    /* create the table structure itself */
    table_set = netsnmp_create_table_data_set("analogInputTable");

    /* comment this out or delete if you don't support creation of new rows */
    table_set->allow_creation = 1;

    /***************************************************
     * Adding indexes
     */
    DEBUGMSGTL(("initialize_table_analogInputTable",
                "adding indexes to table analogInputTable\n"));
    netsnmp_table_set_add_indexes(table_set,
                           ASN_INTEGER,  /* index: analogInputIndex */
                           0);

    DEBUGMSGTL(("initialize_table_analogInputTable",
                "adding column types to table analogInputTable\n"));		 
    netsnmp_table_set_multi_add_default_row(table_set,
                                            COLUMN_ANALOGINPUTINDEX, ASN_INTEGER, 0,
                                            NULL, 0,
                                            COLUMN_ANALOGMEASUREMENTVOLTAGE, ASN_OPAQUE, 0,
                                            NULL, 0,
                                            COLUMN_ANALOGMEASUREMENTCURRENT, ASN_OPAQUE, 0,
                                            NULL, 0,
                              0);
    
    /* registering the table with the master agent */
    /* note: if you don't need a subhandler to deal with any aspects
       of the request, change analogInputTable_handler to "NULL" */
    netsnmp_register_table_data_set(netsnmp_create_handler_registration("analogInputTable", analogInputTable_handler,
                                                        analogInputTable_oid,
                                                        OID_LENGTH(analogInputTable_oid),
                                                        HANDLER_CAN_RWRITE),
                            table_set, NULL);
}

/** Initializes the crate module */
void
init_crate(void)
{

  /* here we initialize all the tables we're planning on supporting */
    initialize_table_outputTable();
    initialize_table_groupsTable();
    initialize_table_moduleTable();
    initialize_table_sensorTable();
    initialize_table_snmpCommunityTable();
    initialize_table_psAuxiliaryTable();
    initialize_table_fanSpeedTable();
    initialize_table_analogInputTable();
}

/** handles requests for the outputTable table, if anything else needs to be done */
int
outputTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {
    /* perform anything here that you need to do.  The requests have
       already been processed by the master table_dataset handler, but
       this gives you chance to act on the request in some other way
       if need be. */
    return SNMP_ERR_NOERROR;
}

/** handles requests for the groupsTable table, if anything else needs to be done */
int
groupsTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {
    /* perform anything here that you need to do.  The requests have
       already been processed by the master table_dataset handler, but
       this gives you chance to act on the request in some other way
       if need be. */
    return SNMP_ERR_NOERROR;
}

/** handles requests for the moduleTable table, if anything else needs to be done */
int
moduleTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {
    /* perform anything here that you need to do.  The requests have
       already been processed by the master table_dataset handler, but
       this gives you chance to act on the request in some other way
       if need be. */
    return SNMP_ERR_NOERROR;
}

/** handles requests for the sensorTable table, if anything else needs to be done */
int
sensorTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {
    /* perform anything here that you need to do.  The requests have
       already been processed by the master table_dataset handler, but
       this gives you chance to act on the request in some other way
       if need be. */
    return SNMP_ERR_NOERROR;
}

/** handles requests for the snmpCommunityTable table, if anything else needs to be done */
int
snmpCommunityTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {
    /* perform anything here that you need to do.  The requests have
       already been processed by the master table_dataset handler, but
       this gives you chance to act on the request in some other way
       if need be. */
    return SNMP_ERR_NOERROR;
}

/** handles requests for the psAuxiliaryTable table, if anything else needs to be done */
int
psAuxiliaryTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {
    /* perform anything here that you need to do.  The requests have
       already been processed by the master table_dataset handler, but
       this gives you chance to act on the request in some other way
       if need be. */
    return SNMP_ERR_NOERROR;
}

/** handles requests for the fanSpeedTable table, if anything else needs to be done */
int
fanSpeedTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {
    /* perform anything here that you need to do.  The requests have
       already been processed by the master table_dataset handler, but
       this gives you chance to act on the request in some other way
       if need be. */
    return SNMP_ERR_NOERROR;
}

/** handles requests for the analogInputTable table, if anything else needs to be done */
int
analogInputTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {
    /* perform anything here that you need to do.  The requests have
       already been processed by the master table_dataset handler, but
       this gives you chance to act on the request in some other way
       if need be. */
    return SNMP_ERR_NOERROR;
}
