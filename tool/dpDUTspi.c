/* ************ MICROSEMI SOC CORP. DIRECTC LICENSE AGREEMENT ************* */
/* ------------------------------------------------------------------------ 
PLEASE READ: BEFORE INSTALLING THIS SOFTWARE, CAREFULLY READ THE FOLLOWING 
MICROSEMI SOC CORP LICENSE AGREEMENT REGARDING THE USE OF THIS SOFTWARE. 
INSTALLING THIS SOFTWARE INDICATES THAT YOU ACCEPT AND UNDERSTAND THIS AGREEMENT 
AND WILL ABIDE BY IT. 

Note: This license agreement (�License�) only includes the following software: 
DirectC. DirectC is licensed under the following terms and conditions.

Hereinafter, Microsemi SoC Corp. shall be referred to as �Licensor� or �Author,� 
whereas the other party to this License shall be referred to as �Licensee.� Each 
party to this License shall be referred to, singularly, as a �Party,� or, 
collectively, as the �Parties.�

Permission to use, copy, modify, and/or distribute DirectC for any purpose, with
or without fee, is hereby granted by Licensor to Licensee, provided that the 
above Copyright notice and this permission notice appear in all copies, 
modifications and/or distributions of DirectC.

DIRECTC IS PROVIDED "AS IS" AND THE AUTHOR/LICENSOR DISCLAIMS ALL WARRANTIES 
WITH REGARD TO DIRECTC INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS. IN NO EVENT SHALL AUTHOR/LICENSOR BE LIABLE TO LICENSEE FOR ANY DAMAGES, 
INCLUDING SPECIAL, DIRECT,INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
WITH THE USE OR PERFORMANCE OF DIRECTC.

Export Control: Information furnished to Licensee may include United States 
origin technical data. Accordingly, Licensee is responsible for complying with, 
and warrants to Licensor that it will comply with, all U.S. export control laws 
and regulations, including the provisions of the Export Administration Act of 
1979 and the Export Administration Regulations promulgated thereunder, the Arms 
Export Control Act, and the sanctions laws administered by the Office of Foreign 
Assets Control including any other U.S. Government regulation applicable to the 
export, re-export, or disclosure of such controlled technical data (or the 
products thereof) to Foreign Nationals, whether within or without the U.S., 
including those employed by, or otherwise associated with, Licensee. Licensee 
shall obtain Licensor�s written consent prior to submitting any request for 
authority to export any such technical data.

ADR: Any dispute between the Parties arising from or related to this License or 
the subject matter hereof, including its validity, construction or performance 
thereunder, shall be exclusively resolved through arbitration by a mutually 
acceptable impartial and neutral arbitrator appointed by the Judicial 
Arbitration and Mediation Services (JAMS) in accordance with its rules and 
procedures. If the Parties are not able to agree on an arbitrator within 10 days 
of the date of request for mediation is served, then JAMS shall appoint an 
arbitrator. Notice of arbitration shall be served and filed with the JAMS main 
offices in Irvine, California. Each Party shall be responsible for all costs 
associated with the preparation and representation by attorneys, or any other 
persons retained thereby, to assist in connection with any such Arbitration. 
However, all costs charged by the mutually agreed upon Arbitration entity shall 
be equally shared by the Parties. The Party seeking Mediation and/or Arbitration 
as provided herein agrees that the venue for any such Mediation and Arbitration 
shall be selected by the other Party and that such venue must be Los Angeles, 
California; New York, New York; or Chicago, Illinois; whereby the applicable law 
and provisions of the Evidence Code of the State selected thereby shall be 
applicable and shall govern the validity, construction and performance of this 
License.

Governing Law: This license will be governed by the laws of the State of 
California, without regard to its conflict of law provisions.

Entire Agreement: This document constitutes the entire agreement between the 
Parties with respect to the subject matter herein and supersedes all other 
communications whether written or oral.                                     */

/* ************************************************************************ */
/*                                                                          */
/*  SPI_DirectC     Copyright (C) Microsemi Corporation                     */
/*  Version 2022.1  Release date July 2022                                  */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpDUTspi.c                                              */
/*                                                                          */
/*  Description:    Contains definitions of common SPI functions            */
/*                                                                          */
/* ************************************************************************ */
#include <stdio.h>
#include "dpuser.h"
#include "dpDUTspi.h"
#include "spi.h" //"mss_spi.h"

DPUCHAR spi_command_buffer;
DPUCHAR spi_hwstatus_buffer;
DPUCHAR spi_input_buffer[SPI_BUFFER_BYTE_SIZE];
DPUCHAR spi_output_buffer[SPI_BUFFER_BYTE_SIZE];

/*******************************************************************************
*
*/
void dut_spi_flash_init(void)
{
    /*--------------------------------------------------------------------------
    * Configure MSS_SPI.
    */
    MSS_SPI_init();
    MSS_SPI_configure_master_mode(
    DUT_SPI_INSTANCE,
    MSS_SPI_SLAVE_0,
    MSS_SPI_MODE3,
    DUT_SPI_PCLK_DIVIDER,
    DUT_MSS_SPI_FRAME_BIT_LENGTH
    );
    return;
}

void dp_flush_spi_buffers(void)
{
    DPUCHAR index;
    spi_command_buffer = 0u;
    for (index = 0u; index < SPI_BUFFER_BYTE_SIZE; index++)
    {
        spi_input_buffer[index] = 0u;
        spi_output_buffer[index] = 0u;
    }

    return;
}

void do_SPI_SCAN_in(unsigned char command, unsigned int data_bits, unsigned char *input_buffer)
{
    unsigned char tmp_buf[17] = {0};
    int i;

    if((((data_bits / 8)+1))>sizeof(tmp_buf))
    {
        printf("Error %s:%d tx[%d]rx[%d]\n", __func__, __LINE__, 1, (data_bits / 8));
        while(1);
    }

    tmp_buf[0] = command;
    for (i=1; (i <= (data_bits/8)) ; i++)
    {
        tmp_buf[i] = input_buffer[i-1];
    }

    MSS_SPI_set_slave_select( DUT_SPI_INSTANCE, DUT_SPI_SLAVE );
    /* writing command + buffer to flash */
    MSS_SPI_transfer_block( DUT_SPI_INSTANCE, tmp_buf,  (1+ (data_bits/8)) /*len*/, DPNULL, 0u);
    for (i=1; (i <= (data_bits/8)) ; i++)
    {
        spi_output_buffer[i-1] = tmp_buf[i];
    }

    MSS_SPI_clear_slave_select( DUT_SPI_INSTANCE, DUT_SPI_SLAVE );

    return;
}

void do_SPI_SCAN_out(unsigned int command_bits, unsigned char *command, unsigned int data_bits, unsigned char *output_buffer)
{
    int i;
    unsigned char tmp_buf[17] = {0};
    if((((data_bits / 8)+(command_bits / 8)))>sizeof(tmp_buf))
    {
        printf("Error %s:%d tx[%d]rx[%d]\n", __func__, __LINE__, (command_bits / 8), (data_bits / 8));
        while(1);
    }

    for (i=0; (i< (command_bits / 8))  ; i++)
    {
        tmp_buf[i] = command[i];
    }


    MSS_SPI_set_slave_select( DUT_SPI_INSTANCE, DUT_SPI_SLAVE );
    /* reading one more byte then the actual rx-len */
    MSS_SPI_transfer_block( DUT_SPI_INSTANCE, (DPUCHAR*)(DPUCHAR*)DPNULL, 0, tmp_buf, (data_bits / 8)+(command_bits / 8));
    MSS_SPI_clear_slave_select( DUT_SPI_INSTANCE, DUT_SPI_SLAVE );

    for (i=0; (i< (command_bits / 8)) ; i++)
    {
        command[i] = tmp_buf[i];
    }

    for (i=(command_bits / 8); (i < ((data_bits / 8)+(command_bits / 8))); i++)
    {
        output_buffer[i-(command_bits / 8)] = tmp_buf[i];
    }

    return;
}

/*   *************** End of File *************** */

