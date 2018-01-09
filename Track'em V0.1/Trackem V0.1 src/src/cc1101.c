#include <util.h>
#include <cc1101.h>







struct cc1101_reg {
    u16_t addr;
    char *name;
};

static const struct cc1101_reg cc1101_reg[] = {
    { .addr = CC1101_IOCFG2,        .name = "IOCFG2"},
    { .addr = CC1101_IOCFG1,        .name = "IOCFG1"},
    { .addr = CC1101_IOCFG0,        .name = "IOCFG0"},
    { .addr = CC1101_FIFOTHR,       .name = "FIFOTHR"},
    { .addr = CC1101_SYNC1,         .name = "SYNC1"},
    { .addr = CC1101_SYNC0,         .name = "SYNC0"},
    { .addr = CC1101_PKTLEN,        .name = "PKTLEN"},
    { .addr = CC1101_PKTCTRL1,      .name = "PKTCTRL1"},
    { .addr = CC1101_PKTCTRL0,      .name = "PKTCTRL0"},
    { .addr = CC1101_ADDR,          .name = "ADDR"},
    { .addr = CC1101_CHANNR,        .name = "CHANNR"},
    { .addr = CC1101_FSCTRL1,       .name = "FSCTRL1"},
    { .addr = CC1101_FSCTRL0,       .name = "FSCTRL0"},
    { .addr = CC1101_FREQ2,         .name = "FREQ2"},
    { .addr = CC1101_FREQ1,         .name = "FREQ1"},
    { .addr = CC1101_FREQ0,         .name = "FREQ0"},
    { .addr = CC1101_MDMCFG4,       .name = "MDMCFG4"},
    { .addr = CC1101_MDMCFG3,       .name = "MDMCFG3"},
    { .addr = CC1101_MDMCFG2,       .name = "MDMCFG2"},
    { .addr = CC1101_MDMCFG1,       .name = "MDMCFG1"},
    { .addr = CC1101_MDMCFG0,       .name = "MDMCFG0"},
    { .addr = CC1101_DEVIATN,       .name = "DEVIATN"},
    { .addr = CC1101_MCSM2,         .name = "MCSM2"},
    { .addr = CC1101_MCSM1,         .name = "MCSM1"},
    { .addr = CC1101_MCSM0,         .name = "MCSM0"},
    { .addr = CC1101_FOCCFG,        .name = "FOCCFG"},
    { .addr = CC1101_BSCFG,         .name = "BSCFG"},
    { .addr = CC1101_AGCCTRL2,      .name = "AGCCTRL2"},
    { .addr = CC1101_AGCCTRL1,      .name = "AGCCTRL1"},
    { .addr = CC1101_AGCCTRL0,      .name = "AGCCTRL0"},
    { .addr = CC1101_WOREVT1,       .name = "WOREVT1"},
    { .addr = CC1101_WOREVT0,       .name = "WOREVT0"},
    { .addr = CC1101_WORCTRL,       .name = "WORCTRL"},
    { .addr = CC1101_FREND1,        .name = "FREND1"},
    { .addr = CC1101_FREND0,        .name = "FREND0"},
    { .addr = CC1101_FSCAL3,        .name = "FSCAL3"},
    { .addr = CC1101_FSCAL2,        .name = "FSCAL2"},
    { .addr = CC1101_FSCAL1,        .name = "FSCAL1"},
    { .addr = CC1101_FSCAL0,        .name = "FSCAL0"},
    { .addr = CC1101_RCCTRL1,       .name = "RCCTRL1"},
    { .addr = CC1101_RCCTRL0,       .name = "RCCTRL0"},
    { .addr = CC1101_FSTEST,        .name = "FSTEST"},
    { .addr = CC1101_PTEST,         .name = "PTEST"},
    { .addr = CC1101_AGCTEST,       .name = "AGCTEST"},
    { .addr = CC1101_TEST2,         .name = "TEST2"},
    { .addr = CC1101_TEST1,         .name = "TEST1"},
    { .addr = CC1101_TEST0,         .name = "TEST0"},
    { .addr = CC1101_PARTNUM,       .name = "PARTNUM"},
    { .addr = CC1101_VERSION,       .name = "VERSION"},
    { .addr = CC1101_FREQEST,       .name = "FREQEST"},
    { .addr = CC1101_LQI,           .name = "LQI"},
    { .addr = CC1101_RSSI,          .name = "RSSI"},
    { .addr = CC1101_MARCSTATE,     .name = "MARCSTATE"},
    { .addr = CC1101_WORTIME1,      .name = "WORTIME1"},
    { .addr = CC1101_WORTIME0,      .name = "WORTIME0"},
    { .addr = CC1101_PKTSTATUS,     .name = "PKTSTATUS"},
    { .addr = CC1101_VCO_VC_DAC,    .name = "VCO_VC_DAC"},
    { .addr = CC1101_TXBYTES,       .name = "TXBYTES"},
    { .addr = CC1101_RXBYTES,       .name = "RXBYTES"},
    { .addr = CC1101_RCCTRL1_STATUS,.name = "RCCTRL1_STATUS"},
    { .addr = CC1101_RCCTRL0_STATUS,.name = "RCCTRL0_STATUS"}

};

void sendreg() {

    volatile u8_t testreg = cc1101_reg[CC1101_FREQ2].addr;

}