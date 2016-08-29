snmpwalk -v 2c -M /usr/share/snmp/mibs -m +WIENER-CRATE-MIB -c guru 192.168.11.11 outputMeasurementTerminalVoltage
snmpwalk -v 2c -M /usr/share/snmp/mibs -m +WIENER-CRATE-MIB -c guru 192.168.11.11 outputMeasurementSenseVoltage
snmpwalk -v 2c -M /usr/share/snmp/mibs -m +WIENER-CRATE-MIB -c guru 192.168.11.11 outputMeasurementCurrent
