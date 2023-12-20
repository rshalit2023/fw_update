/* ************ MICROSEMI SOC CORP. DIRECTC LICENSE AGREEMENT ************* */
/* ------------------------------------------------------------------------ 
PLEASE READ: BEFORE INSTALLING THIS SOFTWARE, CAREFULLY READ THE FOLLOWING 
MICROSEMI SOC CORP LICENSE AGREEMENT REGARDING THE USE OF THIS SOFTWARE. 
INSTALLING THIS SOFTWARE INDICATES THAT YOU ACCEPT AND UNDERSTAND THIS AGREEMENT 
AND WILL ABIDE BY IT. 
כה
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
/*  Module:         dpG5alg.c                                               */
/*                                                                          */
/*  Description:    Contains PolarFire programming functions                */
/*                                                                          */
/* ************************************************************************ */

#include "dpuser.h"
#include "dputil.h"
#include "dpcom.h"
#include "dpalg.h"
#include "dpG5alg.h"
#include "dpG5spi.h"

#ifdef ENABLE_G5M_SUPPORT
DPUCHAR G5M_shared_buf[1024]; /* Maximum of 1024 */
DPUCHAR g5_pgmmode_flag;
DPUCHAR g5_pgmmode;

DPUCHAR g5_component_digest[32];
DPUCHAR g5_component_type;
DPUCHAR g5_componenet_Supports_Cert;

DPUINT  g5_prev_failed_component = 0;
DPULONG g5_prev_failed_block = 0;
DPUINT  g5_prev_unique_error_code = 0;

DPUINT  g5_current_failed_component = 0;
DPULONG g5_current_failed_block = 0;
DPUINT  g5_current_unique_error_code = 0;



/****************************************************************************
* Purpose: main entry function
*  This function needs to be called from the main application function with
*  the approppriate action code set to intiate the desired action.
****************************************************************************/
DPUCHAR dp_top_g5m (void)
{
    dp_G5M_init_vars();
    dp_G5M_check_action();
    if (error_code == DPE_SUCCESS)
    {
        dp_G5M_exit_suspend_mode();
        dp_G5M_perform_action();
    }
    return error_code;
}

void dp_G5M_init_vars(void)
{
    g5_pgmmode_flag = FALSE;
    g5_pgmmode = 0u;
    
    return;
}

void dp_G5M_exit_suspend_mode(void)
{
    /* In order to exit system controller suspend mode, trstb must be held high during programming */
    return;
}

void dp_G5M_check_action(void)
{
    if (! (
    (Action_code == DP_READ_IDCODE_ACTION_CODE) ||
    (Action_code == DP_DEVICE_INFO_ACTION_CODE) ||
    (Action_code == DP_ERASE_ACTION_CODE) ||
    (Action_code == DP_PROGRAM_ACTION_CODE) ||
    (Action_code == DP_REPROGRAM_INFLIGHT_ACTION_CODE) ||
    (Action_code == DP_VERIFY_ACTION_CODE) ||
    (Action_code == DP_ENC_DATA_AUTHENTICATION_ACTION_CODE) ||
    (Action_code == DP_VERIFY_DIGEST_ACTION_CODE) ||
    (Action_code == DP_READ_DEVICE_CERTIFICATE_ACTION_CODE) ||
    (Action_code == DP_ZEROIZE_LIKE_NEW_ACTION_CODE) ||
    (Action_code == DP_ZEROIZE_UNRECOVERABLE_ACTION_CODE)  
    ))
    {
        error_code = DPE_ACTION_NOT_SUPPORTED;
        #ifdef ENABLE_DISPLAY
        dp_display_text("\r\nInvalid action.");
        #endif
    }
    #ifndef ENABLE_DISPLAY
    if ( (Action_code == DP_READ_IDCODE_ACTION_CODE) ||
    (Action_code == DP_DEVICE_INFO_ACTION_CODE) )
    error_code = DPE_ACTION_NOT_SUPPORTED;
    #endif
    
    return;
}

void dp_G5M_perform_action (void)
{
    action_performed = FALSE;
    switch (Action_code)
    {
        case DP_ZEROIZE_LIKE_NEW_ACTION_CODE:
        action_performed = TRUE;
        dp_G5M_zeroize_like_new_action();
        break;     
        case DP_ZEROIZE_UNRECOVERABLE_ACTION_CODE:
        action_performed = TRUE;
        dp_G5M_zeroize_unrecoverable_action();
        break;     
        #ifdef ENABLE_DISPLAY
        case DP_READ_IDCODE_ACTION_CODE:
        action_performed = TRUE;
        dp_read_idcode_action();
        break;        
        case DP_DEVICE_INFO_ACTION_CODE:
        action_performed = TRUE;
        dp_G5M_device_info_action();
        break;
        case DP_READ_DEVICE_CERTIFICATE_ACTION_CODE:
        action_performed = TRUE;
        dp_G5M_read_device_certificate_action();
        break;        
        #endif
    }
    if (action_performed == FALSE)
    {
        dp_G5M_display_bitstream_digest();
        dp_check_image_crc();
        if (error_code == DPE_SUCCESS)
        {
            dp_check_G5_device_ID();
            if (error_code == DPE_SUCCESS)
            {
                switch (Action_code)
                {
                    case DP_ERASE_ACTION_CODE:
                    action_performed = TRUE;
                    dp_G5M_erase_action();
                    break;
                    case DP_PROGRAM_ACTION_CODE:
                    action_performed = TRUE;
                    dp_G5M_program_action();
                    break;
                    case DP_REPROGRAM_INFLIGHT_ACTION_CODE:
                    action_performed = TRUE;
                    dp_G5M_reprogram_inflight_action();
                    break;                        
                    case DP_VERIFY_ACTION_CODE:
                    action_performed = TRUE;
                    dp_G5M_verify_action();
                    break;
                    case DP_ENC_DATA_AUTHENTICATION_ACTION_CODE:
                    action_performed = TRUE;
                    dp_G5M_enc_data_authentication_action();
                    break;
                    case DP_VERIFY_DIGEST_ACTION_CODE:
                    action_performed = TRUE;
                    dp_G5M_verify_digest_action();
                    break;
                }
            }
        }
    }
    dp_G5M_exit();
    return;
}

void dp_G5M_read_idcode(void)
{
    dp_flush_spi_buffers();
    spi_command_buffer = G5M_SPI_READ_IDCODE;
    G5M_SPI_SCAN_out(0u,32u);
    
    device_ID = (DPULONG)spi_output_buffer[0] | (DPULONG)spi_output_buffer[1] << 8u | (DPULONG)spi_output_buffer[2] << 16u | (DPULONG)spi_output_buffer[3] << 24u;
    device_rev = (DPULONG) (device_ID >> 28);
    #ifdef ENABLE_DISPLAY
    dp_display_text("\r\nID read: ");
    dp_display_value(device_ID,HEX);
    #endif
    
    if ((device_ID & 0xfffu) != MICROSEMI_ID)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text("\r\nError: Invalid ID ");
        dp_display_value(device_ID,HEX);
        #endif
        error_code = DPE_IDCODE_ERROR;
    }
    
    return;
}

void dp_check_G5_device_ID (void)
{
    /* DataIndex is a variable used for loading the array data but not used now.  
    * Therefore, it can be used to store the Data file ID for */
    DataIndex = dp_get_bytes(Header_ID,GXM_ID_OFFSET,GXM_ID_BYTE_LENGTH);
    
    global_ulong1 = dp_get_bytes(Header_ID,GXM_ID_MASK_OFFSET,4U);
    device_exception = (DPUCHAR)dp_get_bytes(Header_ID,G5M_DEVICE_EXCEPTION_OFFSET, G5M_DEVICE_EXCEPTION_BYTE_LENGTH);
    
    device_ID &= global_ulong1;
    DataIndex &= global_ulong1;
    
    
    /* Identifying target device and setting its parms */
    
    if ( (DataIndex & 0xfff) == MICROSEMI_ID)
    {
        if (device_ID == DataIndex )
        {
            if ( ( (device_exception == MPF300T_ES_DEVICE_CODE) || (device_exception == MPF300TS_ES_DEVICE_CODE) || (device_exception == MPF300XT_DEVICE_CODE) ) 
            && (device_rev > 4u))
            {
                unique_exit_code = 32857;
                error_code = DPE_IDCODE_ERROR;
                #ifdef ENABLE_DISPLAY
                dp_display_text("\r\nFailed to verify IDCODE");
                dp_display_text("\r\nMPF300(XT|T_ES|TS_ES) programming file is not compatible with MPF300 production devices.");
                dp_display_text("\r\nYou must use a programming file for MPF300(T|TS|TL|TLS) device.");
                dp_display_text("\r\nERROR_CODE: ");
                dp_display_value(unique_exit_code, HEX);
                #endif
            }          
            else if ( ( (device_exception == MPF300T_DEVICE_CODE) || (device_exception == MPF300TS_DEVICE_CODE) || (device_exception == MPF300TL_DEVICE_CODE) || (device_exception == MPF300TLS_DEVICE_CODE)) 
            && (device_rev < 5u))
            {
                unique_exit_code = 32858;
                error_code = DPE_IDCODE_ERROR;
                #ifdef ENABLE_DISPLAY
                dp_display_text("\r\nFailed to verify IDCODE");
                dp_display_text("\r\nMPF300(T|TS|TL|TLS) programming file is not compatible with MPF300(XT|T_ES|TS_ES) devices.");
                dp_display_text("\r\nYou must use a programming file for MPF300(XT|T_ES|TS_ES) device.");
                dp_display_text("\r\nERROR_CODE: ");
                dp_display_value(unique_exit_code, HEX);
                #endif
            }          
            else
            {
                device_family = (DPUCHAR) dp_get_bytes(Header_ID,GXM_DEVICE_FAMILY_OFFSET,GXM_DEVICE_FAMILY_BYTE_LENGTH);
                #ifdef ENABLE_DISPLAY
                dp_display_text("\r\nActID = ");
                dp_display_value(device_ID,HEX);
                dp_display_text(" ExpID = ");
                dp_display_value(DataIndex,HEX);
                dp_display_text("\r\nDevice Rev = ");
                dp_display_value(device_rev,HEX);
                dp_display_text("\r\nDevice Family = ");
                dp_display_value(device_family,HEX);
                #endif
            }
        }
        else
        {
            error_code = DPE_IDCODE_ERROR;
            unique_exit_code = 32772;
            #ifdef ENABLE_DISPLAY
            dp_display_text(" ExpID = ");
            dp_display_value(DataIndex,HEX);
            dp_display_text("\r\nERROR_CODE: ");
            dp_display_value(unique_exit_code, HEX);
            #endif
        }
    }
    else
    {
        error_code = DPE_IDCODE_ERROR;
    }
    
    return;
}

void dp_G5M_erase_action(void)
{
    #ifdef ENABLE_DISPLAY
    dp_display_text("\r\nPerforming ERASE action: ");
    #endif
    dp_G5M_initialize();
    #ifdef ENABLE_DISPLAY
    dp_display_text("\r\nErasing the device takes few seconds.  Please wait...");
    #endif
    
    if (error_code == DPE_SUCCESS)
    {
        g5_pgmmode = 0x1u;
        dp_G5M_set_mode();
        
        
        if (error_code == DPE_SUCCESS)
        {
            /* Global unit1 is used to hold the number of components */
            global_uint1 = (DPUINT)dp_get_bytes(Header_ID,GXM_NUMOFCOMPONENT_OFFSET,GXM_NUMOFCOMPONENT_BYTE_LENGTH);
            global_uint2 = global_uint1 - ((DPUINT)dp_get_bytes(Header_ID,GXM_ERASEDATASIZE_OFFSET,GXM_DATASIZE_BYTE_LENGTH) - 1u);
            dp_G5M_process_data(GXM_erasedatastream_ID);
            if(error_code != DPE_SUCCESS)
            {
                error_code = DPE_ERASE_ERROR;
            }
        }
    }
    return;
}

void dp_G5M_reprogram_inflight_action(void)
{
    DPUCHAR done = 0u;
    DPUINT pgm_attempts = 0;
    DPUINT vfy_attempts = 0;
    
    #ifdef ENABLE_DISPLAY
    dp_display_text("\r\nPerforming Reprogram_inFlight action: ");
    #endif  
    
    dp_G5M_clear_errors();
    
    dp_G5M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        while ((!done) && (pgm_attempts < G5M_MAX_PROGRAM_ATTEMPTS))
        {
            error_code = DPE_SUCCESS;
            vfy_attempts = 0;
            pgm_attempts++;
            dp_G5M_do_program();
            if (error_code != DPE_SUCCESS)
            {
                // If the previous programming run failed on the same location, abort
                if (
                (g5_current_failed_component == g5_prev_failed_component) &&
                (g5_current_failed_block == g5_prev_failed_block) &&
                (g5_current_unique_error_code == g5_prev_unique_error_code)
                )
                {
                    break;
                }
                else // Otherwise, save the error and try again
                {
                    g5_prev_failed_component = g5_current_failed_component;
                    g5_prev_failed_block = g5_current_failed_block;
                    g5_prev_unique_error_code = g5_current_unique_error_code;
                }
            }
            else
            {
                dp_G5M_clear_errors();
                
                while ((!done) && (vfy_attempts < G5M_MAX_VERIFY_ATTEMPTS))
                {
                    error_code = DPE_SUCCESS;                  
                    vfy_attempts++;
                    dp_G5M_do_verify();
                    if (error_code == DPE_SUCCESS)
                    {
                        done = 1u;
                    }
                    else if ( // If the error is the same as the previous error, abort.  It will attempt to reprogram
                    (g5_current_failed_component == g5_prev_failed_component) &&
                    (g5_current_failed_block == g5_prev_failed_block) &&
                    (g5_current_unique_error_code == g5_prev_unique_error_code)
                    )
                    {
                        dp_G5M_clear_errors();
                        break;
                    }
                    else
                    { // Otherwise, save the errors and try verify again.
                        g5_prev_failed_component = g5_current_failed_component;
                        g5_prev_failed_block = g5_current_failed_block;
                        g5_prev_unique_error_code = g5_current_unique_error_code;
                    }
                }
            }
        }
    }
    return;
}

void dp_G5M_clear_errors(void)
{
    g5_prev_failed_component = 0;
    g5_current_failed_component = 0;
    g5_prev_failed_block = 0;
    g5_current_failed_block = 0;
    
    return;
}


void dp_G5M_program_action(void)
{
    #ifdef ENABLE_DISPLAY
    dp_display_text("\r\nPerforming PROGORAM action: ");  
    #endif
    dp_G5M_initialize();
    #ifdef ENABLE_DISPLAY
    dp_display_text("\r\nProgramming...");
    #endif
    if (error_code == DPE_SUCCESS)
    {
        dp_G5M_do_program();
    }
    
    return;
}

void dp_G5M_do_program(void)
{
    #ifdef ENABLE_DISPLAY  
    dp_display_text("\r\nPerforming stand alone program...");
    #endif
    dp_G5M_check_cycle_count(); 
    if (error_code == DPE_SUCCESS)
    {
        g5_pgmmode = 0x1u;
        dp_G5M_set_mode();
        
        
        if (error_code == DPE_SUCCESS)
        {
            /* Global unit1 is used to hold the number of components */
            global_uint1 = (DPUINT)dp_get_bytes(GXM_Header_ID,GXM_DATASIZE_OFFSET,GXM_DATASIZE_BYTE_LENGTH);
            global_uint2 = 1u;
            
            dp_G5M_process_data(GXM_datastream_ID);
            if(error_code != DPE_SUCCESS)
            {
                error_code = DPE_PROGRAM_ERROR;
                #ifdef ENABLE_DISPLAY  
                dp_display_text("program err");
                #endif
            }
        }
        else{
            #ifdef ENABLE_DISPLAY  
            dp_display_text("program err");
            #endif
        }
    }
    else{
            #ifdef ENABLE_DISPLAY  
            dp_display_text("program err");
            #endif
        }
    return;
}

void dp_G5M_verify_action(void)
{
    #ifdef ENABLE_DISPLAY
    dp_display_text("\r\nPerforming VERIFY action: ");  
    #endif
    dp_G5M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        dp_G5M_do_verify();
    }
    
    return;
}


void dp_G5M_do_verify(void)
{
    #ifdef ENABLE_DISPLAY    
    dp_display_text("\r\nPerforming stand alone verify...");
    #endif
    
    g5_pgmmode = 0x2u;
    dp_G5M_set_mode();
    
    
    if (error_code == DPE_SUCCESS)
    {
        /* Global unit1 is used to hold the number of components */
        global_uint1 = (DPUINT)dp_get_bytes(GXM_Header_ID,GXM_DATASIZE_OFFSET,GXM_DATASIZE_BYTE_LENGTH);
        global_uint2 = 1u;
        
        dp_G5M_process_data(GXM_datastream_ID);
        if(error_code != DPE_SUCCESS)
        {
            error_code = DPE_PROGRAM_ERROR;
        }
    }
    return;
}

void dp_G5M_enc_data_authentication_action(void)
{
    #ifdef ENABLE_DISPLAY
    dp_display_text("\r\nPerforming AUTHENTICATION action: ");  
    #endif
    dp_G5M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        g5_pgmmode = 0x0u;
        dp_G5M_set_mode();
        
        
        if (error_code == DPE_SUCCESS)
        {
            /* Global unit1 is used to hold the number of components */
            global_uint1 = (DPUINT)dp_get_bytes(GXM_Header_ID,GXM_DATASIZE_OFFSET,GXM_DATASIZE_BYTE_LENGTH);
            global_uint2 = 1u;
            
            dp_G5M_process_data(GXM_datastream_ID);
            if(error_code != DPE_SUCCESS)
            {
                error_code = DPE_AUTHENTICATION_FAILURE;
            }
        }
    }
    return;
}

void dp_G5M_verify_digest_action(void)
{
    #ifdef ENABLE_DISPLAY 
    dp_display_text("\r\nPerforming VERIFY_DIGEST action: ");
    #endif
    dp_G5M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        dp_G5M_query_security();
    }
    if ((error_code == DPE_SUCCESS) && ((G5M_shared_buf[1] & G5M_UL_EXTERNAL_DIGEST_CHECK) == G5M_UL_EXTERNAL_DIGEST_CHECK) )
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text("r\nExternal digest check via JTAG/SPI Slave is disabled.");
        #endif
        error_code = DPE_VERIFY_DIGEST_ERROR;
    }
    
    if (error_code == DPE_SUCCESS)
    {
        dp_flush_spi_buffers();
        spi_input_buffer[0] = 0x1u;
        if (device_family == G5M_DEVICE_FAMILY)
        {
            spi_command_buffer = G5M_CHECK_DIGESTS;
            G5M_SPI_SCAN_out(16u,16u);
        }
        else
        {
            spi_command_buffer = G5MSOC_CHECK_DIGESTS;
            G5M_SPI_SCAN_out(32u,32u);
        }
        
        if ((device_family == G5M_DEVICE_FAMILY) && (spi_output_buffer[1] == 0x40u))
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text("\r\nFailed to verify digest.");
            #endif
            error_code = DPE_VERIFY_DIGEST_ERROR;
        }
        else if ((device_family == G5SOC_DEVICE_FAMILY) && (spi_output_buffer[3] == 0x40u))
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text("\r\nFailed to verify digest.");
            #endif
            error_code = DPE_VERIFY_DIGEST_ERROR;
        }
        else 
        {
            #ifdef ENABLE_DISPLAY
            if ((spi_output_buffer[0] & 0x1u )== 0x1u)
            {
                dp_display_text("\r\n --- FPGA Fabric digest verification: PASS");
            }
            else
            {
                dp_display_text("\r\nWarning: --- FPGA Fabric digest verification: FAIL");
            }
            if ((spi_output_buffer[0] & 0x2u )== 0x2u)
            {
                dp_display_text("\r\n --- Fabric Configuration digest verification: PASS");
            }
            else
            {
                dp_display_text("\r\nWarning: --- Fabric Configuration digest verification: FAIL");
            }
            if ((spi_output_buffer[0] & 0x4u )== 0x4u)
            {
                dp_display_text("\r\n --- sNVM digest verification: PASS");
            }
            else
            {
                dp_display_text("\r\nWarning: --- sNVM digest verification: FAIL");
            }
            if (device_family == G5SOC_DEVICE_FAMILY)
            {
                if ((spi_output_buffer[1] & 0x40u )== 0x4u)
                {
                    dp_display_text("\r\n --- eNVM digest verification: PASS");
                }
                else
                {
                    dp_display_text("\r\nWarning: --- eNVM digest verification: FAIL");
                }
            }
            if ((spi_output_buffer[0] & 0x8u )== 0x8u)
            {
                dp_display_text("\r\n --- User security policies digest verification: PASS" );
            }
            else
            {
                dp_display_text("\r\nWarning: --- User security policies digest verification: FAIL" );
            }
            if ((spi_output_buffer[0] & 0x40u )== 0x40u)
            {
                dp_display_text("\r\n --- UPK1 digest verification: PASS" );
            }
            else
            {
                dp_display_text("\r\nWarning: --- UPK1 digest verification: FAIL" );
            }
            if ((spi_output_buffer[0] & 0x80u )== 0x80u)
            {
                dp_display_text("\r\n --- UEK1 digest verification: PASS" );
            }
            else
            {
                dp_display_text("\r\nWarning: --- UEK1 digest verification: FAIL" );
            }
            if ((spi_output_buffer[1] & 0x1u )== 0x1u)
            {
                dp_display_text("\r\n --- DPK digest verification: PASS" );
            }
            else
            {
                dp_display_text("\r\nWarning: --- DPK digest verification: FAIL" );
            }
            if ((spi_output_buffer[0] & 0x10u )== 0x10u)
            {
                dp_display_text("\r\n --- SMK digest verification: PASS");
            }
            else
            {
                dp_display_text("\r\nWarning: --- SMK digest verification: FAIL");
            }
            if ((spi_output_buffer[0] & 0x20u )== 0x20u)
            {
                dp_display_text("\r\n --- User Public Key digest verification: PASS");
            }
            else
            {
                dp_display_text("\r\nWarning: --- User Public Key digest verification: FAIL");
            }
            if ((spi_output_buffer[1] & 0x2u )== 0x2u)
            {
                dp_display_text("\r\n --- UPK2 digest verification: PASS" );
            }
            else
            {
                dp_display_text("\r\nWarning: --- UPK2 digest verification: FAIL" );
            }
            if ((spi_output_buffer[1] & 0x4u )== 0x4u)
            {
                dp_display_text("\r\n --- UEK2 digest verification: PASS" );
            }
            else
            {
                dp_display_text("\r\nWarning: --- UEK2 digest verification: FAIL" );
            }
            if ((spi_output_buffer[1] & 0x10u )== 0x10u)
            {
                dp_display_text("\r\n --- Factory row and factory key segment digest verification: PASS");
            }
            else
            {
                dp_display_text("\r\nWarning: --- Factory row and factory key segment digest verification: FAIL");
            }
            #endif
        }
    }
    return;   
}

void dp_G5M_zeroize_like_new_action(void)
{
    #ifdef ENABLE_DISPLAY
    dp_display_text("\r\nPerforming ZEROIZE_LIKE_NEW action: ");  
    #endif
    dp_G5M_query_security();
    if ((error_code == DPE_SUCCESS) && ((G5M_shared_buf[7] & G5M_UL_USER_KEY1) == G5M_UL_USER_KEY1) )
    {
        dp_G5M_unlock_upk1();
    }
    if ((error_code == DPE_SUCCESS) && ((G5M_shared_buf[7] & G5M_UL_USER_KEY2) == G5M_UL_USER_KEY2) )
    {
        dp_G5M_unlock_upk2();
    }
    if (error_code == DPE_SUCCESS)
    {
        dp_G5M_do_zeroize(1);
    }
    return;
}

void dp_G5M_zeroize_unrecoverable_action(void)
{
    #ifdef ENABLE_DISPLAY
    dp_display_text("\r\nPerforming ZEROIZE_UNRECOVERABLE action: ");
    #endif  
    dp_G5M_query_security();
    if ((error_code == DPE_SUCCESS) && ((G5M_shared_buf[7] & G5M_UL_USER_KEY1) == G5M_UL_USER_KEY1) )
    {
        dp_G5M_unlock_upk1();
    }
    if ((error_code == DPE_SUCCESS) && ((G5M_shared_buf[7] & G5M_UL_USER_KEY2) == G5M_UL_USER_KEY2) )
    {
        dp_G5M_unlock_upk2();
    }
    if (error_code == DPE_SUCCESS)
    {
        dp_G5M_do_zeroize(3);
    }
    return;
}

void dp_G5M_read_device_certificate_action(void)
{
    dp_G5M_read_certificate();
}

void dp_G5M_check_cycle_count(void)
{
    DPUINT cycle_count = 0;
    dp_flush_spi_buffers();
    spi_command_buffer = G5M_SPI_READ_DEBUG_INFO;
    G5M_SPI_SCAN_in(0u);
    
    dp_G5M_read_shared_buffer(6u);
    
    cycle_count = ((G5M_shared_buf[61] << 8u) | (G5M_shared_buf[60]));
    #ifdef ENABLE_DISPLAY
    
    dp_display_text("\r\nCYCLE COUNT: ");
    if (cycle_count != 0xffffu)
    {
        dp_display_value(cycle_count,DEC);
        if (cycle_count > G5M_MAX_ALLOWED_PROGRAMMING_CYCLES)
            dp_display_text("\r\n***** WARNING: MAXIMUM ALLOWED PROGRAMMING CYCLE COUNT IS REACHED *****");
    }
    else
    {
        dp_display_text(" Not available. ");
    }
    #endif
    
    return;
}


#ifdef ENABLE_DISPLAY
void dp_G5M_device_info_action(void)
{
    dp_G5M_read_udv();
    if (error_code == DPE_SUCCESS)
    {
        dp_G5M_read_user_code();
        if (error_code == DPE_SUCCESS)
        {
            dp_G5M_read_design_info();
            if (error_code == DPE_SUCCESS)
            {
                dp_G5M_read_digests();
                if (error_code == DPE_SUCCESS)
                {
                    dp_G5M_read_debug_info();
                    dp_G5M_dump_debug_info();
                    if (error_code == DPE_SUCCESS)
                    {
                        dp_G5M_read_tvs_monitor();
                        if (error_code == DPE_SUCCESS)
                        {
                            dp_G5M_read_fsn();
                            if (error_code == DPE_SUCCESS)
                            {
                                dp_G5M_query_security();
                                dp_G5M_dump_security();
                                if (error_code == DPE_SUCCESS)
                                {                                
                                    if (device_family == G5M_DEVICE_FAMILY)
                                        dp_G5M_read_dibs();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return;
}

void dp_G5M_read_udv(void)
{
    dp_flush_spi_buffers();
    spi_command_buffer = G5M_SPI_UDV;
    G5M_SPI_SCAN_out(0u,32u);
    
    dp_display_text("\r\nUDV: ");
    dp_display_array(spi_output_buffer,4u,HEX);
    
    return;
}

void dp_G5M_read_user_code(void)
{
    dp_flush_spi_buffers();
    spi_command_buffer = G5M_SPI_READ_USERCODE;
    G5M_SPI_SCAN_out(0u,32u);
    
    dp_display_text("\r\nUser Code = ");
    dp_display_array(spi_output_buffer,4u,HEX);
    
    return;
}

void dp_G5M_read_dibs(void)
{
    DPUCHAR dibs_in[16] = {0xB4, 0x70, 0xD8, 0x05, 0x01, 0x4F, 0x1C, 0x77, 0xDE, 0x47, 0x9E, 0xCE, 0x6A, 0x31, 0x72, 0x5C};
    DPUCHAR tmp;
    dp_flush_spi_buffers();
    
    
    spi_command_buffer = G5M_READ_DEVICE_INTEGRITY;
    for (tmp = 0; tmp < 16; tmp++)
        spi_input_buffer[tmp] = dibs_in[tmp];
    
    spi_command_buffer = G5M_SPI_ISC_ENABLE;
    
    G5M_SPI_SCAN_out(128u, 128u);
    
    if ((error_code != DPE_SUCCESS) || ((spi_output_buffer[0]&0x1u) == 0x1u))
    {
        error_code = DPE_POLL_ERROR;
        unique_exit_code = 33003;
        #ifdef ENABLE_DISPLAY
        dp_display_text( "\r\nFailed to read device integrity bits.");
        dp_display_text("\r\nERROR_CODE: ");
        dp_display_value(unique_exit_code, HEX);
        #endif
    }
    else
    {
        #ifdef ENABLE_DISPLAY      
        dp_display_text("\r\nDevice Integrity Bits: ");
        dp_display_array(spi_output_buffer, 32, HEX);
        #endif
    }
    
    return;
}

void dp_G5M_read_design_info(void)
{
    dp_flush_spi_buffers();
    spi_command_buffer = G5M_SPI_READ_DESIGN_INFO;
    G5M_SPI_SCAN_in(0u);
    
    dp_G5M_read_shared_buffer(3u);
    
    
    if (error_code == DPE_SUCCESS)
    {
        dp_display_text("\r\nDesign Name: ");
        for (global_uchar1 = 2u; global_uchar1 < 32u; global_uchar1++)
        {
            dp_display_value(G5M_shared_buf[global_uchar1],CHR);
        }
        dp_display_text("\r\nChecksum: ");
        dp_display_array(G5M_shared_buf,2u,HEX);
        dp_display_text("\r\nDesign Info: ");
        dp_display_array(G5M_shared_buf,34u,HEX);
        dp_display_text("\r\nDESIGNVER: ");
        dp_display_array(&G5M_shared_buf[32],2u,HEX);
        dp_display_text("\r\nBACKLEVEL: ");
        dp_display_array(&G5M_shared_buf[34],2u,HEX);
    }
    
    return;
}

void dp_G5M_read_digests(void)
{
    dp_flush_spi_buffers();
    spi_command_buffer = G5M_SPI_READ_DIGEST;
    G5M_SPI_SCAN_in(0u);
    
    if (device_family == G5M_DEVICE_FAMILY)
        dp_G5M_read_shared_buffer(26u);
    else
    dp_G5M_read_shared_buffer(36u);
    if (error_code == DPE_SUCCESS)
    {
        dp_display_text("\r\nFabric digest: ");
        dp_display_array(&G5M_shared_buf[0],32u,HEX);
        
        dp_display_text("\r\nUFS CC segment digest: ");
        dp_display_array(&G5M_shared_buf[32],32u,HEX);
        
        dp_display_text("\r\nSNVM digest: ");
        dp_display_array(&G5M_shared_buf[64],32u,HEX);
        
        dp_display_text("\r\nUFS UL digest: ");
        dp_display_array(&G5M_shared_buf[96],32u,HEX);
        
        dp_display_text("\r\nUser Key digest 0: ");
        dp_display_array(&G5M_shared_buf[128],32u,HEX);
        
        dp_display_text("\r\nUser Key digest 1: ");
        dp_display_array(&G5M_shared_buf[160],32u,HEX);
        
        dp_display_text("\r\nUser Key digest 2: ");
        dp_display_array(&G5M_shared_buf[192],32u,HEX);
        
        dp_display_text("\r\nUser Key digest 3: ");
        dp_display_array(&G5M_shared_buf[224],32u,HEX);
        
        dp_display_text("\r\nUser Key digest 4: ");
        dp_display_array(&G5M_shared_buf[256],32u,HEX);
        
        dp_display_text("\r\nUser Key digest 5: ");
        dp_display_array(&G5M_shared_buf[288],32u,HEX);
        
        dp_display_text("\r\nUser Key digest 6: ");
        dp_display_array(&G5M_shared_buf[320],32u,HEX);
        
        dp_display_text("\r\nUFS UPERM segment digest: ");
        dp_display_array(&G5M_shared_buf[352],32u,HEX);
        
        dp_display_text("\r\nFactory digest: ");
        dp_display_array(&G5M_shared_buf[384],32u,HEX);
        if (device_family == G5SOC_DEVICE_FAMILY)
        {
            dp_display_text("\r\nUser Key digest 7: ");
            dp_display_array(&G5M_shared_buf[416],32u,HEX); 
            dp_display_text("\r\nENVM Digest: ");
            dp_display_array(&G5M_shared_buf[448],32u,HEX); 
            dp_display_text("\r\nMSS Boot Info digest: ");
            dp_display_array(&G5M_shared_buf[480],32u,HEX);
            dp_display_text("\r\nSNVM RW Access Map: ");
            dp_display_array(&G5M_shared_buf[512],32u,HEX);
            dp_display_text("\r\nSBIC Revocation digest: ");
            dp_display_array(&G5M_shared_buf[544],32u,HEX);
        }
    }
    
    
    return;
}
#endif

void dp_G5M_read_debug_info(void)
{
    dp_flush_spi_buffers();
    spi_command_buffer = G5M_SPI_READ_DEBUG_INFO;
    G5M_SPI_SCAN_in(0u);
    
    dp_G5M_read_shared_buffer(6u);
    if (error_code == DPE_SUCCESS)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text("\r\nDebug Info: ");
        if (device_family == G5SOC_DEVICE_FAMILY)
        {
            dp_display_array(G5M_shared_buf,94u,HEX);
        }
        else
        {
            dp_display_array(G5M_shared_buf,84u,HEX);
        }
        #endif
    }
    
    return;
}

#ifdef ENABLE_DISPLAY
void dp_G5M_dump_debug_info(void)
{
    global_uint1 = ((G5M_shared_buf[61] << 8u) | (G5M_shared_buf[60]));
    dp_display_text("\r\nCycle Count: ");
    dp_display_value(global_uint1,DEC);
    
    if (G5M_shared_buf[36] == 1u)
    {
        dp_display_text("\r\nProgramming mode: JTAG");
    }
    else if (G5M_shared_buf[36] == 2u)
    {
        dp_display_text("\r\nProgramming mode: IAP");
    }
    else if (G5M_shared_buf[36] == 3u)
    {
        dp_display_text("\r\nProgramming mode: SPI-Slave");
    }
    
    if (((G5M_shared_buf[32] &0x3fu) != 0) && ((G5M_shared_buf[32] &0x3fu) != 0x3fu))
    {
        dp_display_text("\r\nAlgorithm version: ");
        dp_display_value(G5M_shared_buf[32] & 0x3fu, DEC);
    }
    return;
}

void dp_G5M_read_tvs_monitor(void)
{
    dp_flush_spi_buffers();
    spi_command_buffer = G5M_SPI_TVS_MONITOR;
    G5M_SPI_SCAN_out(0u,SPI_BUFFER_BIT_SIZE);  
    if (error_code != DPE_SUCCESS)
    {
        error_code = DPE_MATCH_ERROR;
        unique_exit_code = 32846;
        #ifdef ENABLE_DISPLAY
        dp_display_text("\r\nFailed to read tvs monitor.");
        dp_display_text("\r\nERROR_CODE: ");
        dp_display_value(unique_exit_code, HEX);
        #endif
    }
    else
    {
        dp_display_text("\r\nTVS_MONITOR: ");
        dp_display_array(spi_output_buffer,SPI_BUFFER_BYTE_SIZE,HEX);
    }
    
    return;
}


void dp_G5M_read_fsn(void)
{
    dp_flush_spi_buffers();
    spi_command_buffer = G5M_SPI_READ_SN;
    G5M_SPI_SCAN_out(0u,SPI_BUFFER_BIT_SIZE);
    if ((error_code != DPE_SUCCESS) && (unique_exit_code == DPE_SUCCESS))
    {
        unique_exit_code = 32769;
        dp_display_text("\r\nFailed to read DSN.\r\nERROR_CODE: ");
        dp_display_value(unique_exit_code, HEX);
    }
    else
    {    
        dp_display_text("\r\n======================================");
        dp_display_text("\r\nDSN: ");
        dp_display_array(spi_output_buffer, SPI_BUFFER_BYTE_SIZE, HEX);
        dp_display_text("\r\n======================================");
    }
    return;
}
#endif

void dp_G5M_read_shared_buffer(DPUCHAR ucNumOfBlocks)
{
    DPUCHAR tmp_buf1, tmp_buf2;
    for (tmp_buf1 = 0u; tmp_buf1 < ucNumOfBlocks; tmp_buf1++)
    {
        dp_flush_spi_buffers();
        spi_command_buffer = G5M_SPI_READ_BUFFER;
        spi_input_buffer[0] = tmp_buf1;
        G5M_SPI_SCAN_out(SPI_BUFFER_BIT_SIZE,SPI_BUFFER_BIT_SIZE);
        
        for (tmp_buf2 = 0;tmp_buf2 < SPI_BUFFER_BYTE_SIZE; tmp_buf2++)
        {
            G5M_shared_buf[tmp_buf1 * SPI_BUFFER_BYTE_SIZE + tmp_buf2] = spi_output_buffer[tmp_buf2];
        }
    }
    
    return;
}

void dp_G5M_perform_isc_enable(void)
{
    g5_pgmmode_flag = TRUE;
    dp_flush_spi_buffers();
    spi_input_buffer[0] |= (G5M_ALGO_VERSION & 0x3fu);
    spi_input_buffer[2] |= (G5M_DIRECTC_VERSION & 0x3fu) << 1u;
    spi_input_buffer[2] |= (DIRECTC_PROGRAMMING & 0x7u) << 7u;
    spi_input_buffer[3] |= (DIRECTC_PROGRAMMING & 0x7u) >> 1u;
    spi_input_buffer[3] |= 0x28u;
    
    spi_command_buffer = G5M_SPI_ISC_ENABLE;
    G5M_SPI_SCAN_out(32u,32u);
    
    if ((spi_output_buffer[0] & 0x1) == 1)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text("\r\nFailed to enter ming mode.");
        #endif
        error_code = DPE_INIT_FAILURE;
    }
    #ifdef ENABLE_DISPLAY
    dp_display_text("\r\nISC_ENABLE_RESULT: ");
    dp_display_array(spi_output_buffer,4u,HEX);
    #endif
    
    
    
    /* Display CRCERR */
    global_uchar1 = spi_output_buffer[0] & 0x1u;
    #ifdef ENABLE_DISPLAY
    dp_display_text("\r\nCRCERR: ");
    dp_display_value(global_uchar1,HEX);
    #endif
    
    
    return;
}

/* Enter programming mode */
void dp_G5M_initialize(void)
{
    dp_G5M_query_security();
    if ((error_code == DPE_SUCCESS) && ((G5M_shared_buf[7] & G5M_UL_USER_KEY1) == G5M_UL_USER_KEY1) )
    {
        dp_G5M_unlock_upk1();
    }
    if ((error_code == DPE_SUCCESS) && ((G5M_shared_buf[7] & G5M_UL_USER_KEY2) == G5M_UL_USER_KEY2) )
    {
        dp_G5M_unlock_upk2();
    }
    if (error_code == DPE_SUCCESS)
    {
        dp_G5M_perform_isc_enable();
    }
    
    return;
}


/* Function is used to exit programming mode */
void dp_G5M_exit(void)
{
    if (g5_pgmmode_flag == TRUE)
    {
        dp_flush_spi_buffers();
        spi_command_buffer = G5M_SPI_ISC_DISABLE;
        G5M_SPI_SCAN_in(0u);
        dp_delay(1000u);
    }
    #ifdef ENABLE_DISPLAY
    dp_G5M_read_fsn();
    #endif
    
    spi_command_buffer = G5M_SPI_RELEASE;
    G5M_SPI_SCAN_in(0);
    
    spi_command_buffer = G5M_SPI_RELEASE;
    G5M_SPI_SCAN_in(0);
    
    return;
}

void dp_G5M_set_mode(void)
{
    dp_flush_spi_buffers();
    spi_command_buffer = G5M_SPI_FRAME_INIT;
    spi_input_buffer[0] = g5_pgmmode;
    G5M_SPI_SCAN_in(8u);
    
    if (error_code != DPE_SUCCESS)
    {
        unique_exit_code = 32770;
        #ifdef ENABLE_DISPLAY
        dp_display_text("r\nFailed to set programming mode.");
        dp_display_text("\r\nERROR_CODE: ");
        dp_display_value(unique_exit_code, HEX);
        #endif
    }
    
    
    return;
}


void dp_G5M_process_data(DPUCHAR BlockID)
{
    DPUCHAR tmp_buf;
    #ifdef ENABLE_DISPLAY
    /* Global unit1 is used to hold the number of components */
    /* Loop through the number of components */
    dp_display_text("\r\n");
    #endif
    
    
    DataIndex = 0u;
    for (; global_uint2 <= global_uint1; global_uint2++)
    {
        /* get the number of blocks */
        /* Global ulong1 is used to hold the number of blocks within the components */
        global_ulong1 = dp_get_bytes(GXM_NUMBER_OF_BLOCKS_ID,(DPULONG)(((global_uint2 - 1u) * 22u) / 8u),4u);
        global_ulong1 >>= ((global_uint2 - 1U)* 22u) % 8u;
        global_ulong1 &= 0x3FFFFFu;
        
        g5_component_type = (DPUCHAR) dp_get_bytes(GXM_datastream_ID,G5M_COMPONENT_TYPE_IN_HEADER_BYTE+DataIndex/8,1);
        g5_componenet_Supports_Cert = (DPUCHAR) dp_get_bytes(GXM_datastream_ID,G5M_GEN_CERT_BYTE + DataIndex / 8,1) & 0x2u;
        
        #ifdef ENABLE_DISPLAY
        dp_display_text("\r\nProcessing component ");
        dp_display_value(global_uint2,DEC);
        dp_display_text(". Please wait...");
        old_progress = 0;
        #endif
        
        for (global_ulong2 = 1u; global_ulong2 <= global_ulong1; global_ulong2++)
        {
            #ifdef ENABLE_DISPLAY
            new_progress = (global_ulong2 *100 / global_ulong1);
            if (new_progress != old_progress)
            {
                dp_report_progress(new_progress);
                old_progress = new_progress;
            }
            #endif
            
            spi_command_buffer = G5M_SPI_FRAME_DATA;
            dp_G5M_get_and_shift_in(BlockID, SPI_BUFFER_BIT_SIZE, DataIndex);
            
            if ( error_code != DPE_SUCCESS )
            {
                dp_G5M_get_data_status();
                #ifdef ENABLE_DISPLAY            
                dp_display_text("\r\ncomponentNo: ");
                dp_display_value(global_uint2, DEC);
                dp_display_text("\r\nblockNo: ");
                dp_display_value(global_ulong2, DEC);
                dp_display_text("\r\nDATA_STATUS_RESULT: ");
                dp_display_array(spi_output_buffer,4u,HEX);
                dp_display_text("\r\nERRORCODE: ");
                dp_display_value(spi_output_buffer[1],HEX);
                #endif
                if ((spi_output_buffer[1] == 1u) || (spi_output_buffer[1] == 2u) || (spi_output_buffer[1] == 4u) || (spi_output_buffer[1] == 8u) || 
                (spi_output_buffer[1] == 127u) || (spi_output_buffer[1] == 132u) || (spi_output_buffer[1] == 133u) || (spi_output_buffer[1] == 134u) || (spi_output_buffer[1] == 135u)
                )
                {
                    unique_exit_code = 32799;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text("\r\nBitstream or data is corrupted or noisy.");
                    #endif
                }
                else if(spi_output_buffer[1] == 3u)
                {
                    unique_exit_code = 32801;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text("\r\nInvalid/Corrupted encryption key." );
                    #endif
                }
                else if(spi_output_buffer[1] == 5u)
                {
                    unique_exit_code = 32803;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text("\r\nBack level not satisfied." );
                    #endif
                }
                else if(spi_output_buffer[1] == 6u)
                {
                    unique_exit_code = 32847;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text("\r\nBitstream programming action is disabled." );
                    #endif
                }                    
                else if(spi_output_buffer[1] == 7u)
                {
                    unique_exit_code = 32805;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text("\r\nDSN binding mismatch." );
                    #endif
                }
                else if(spi_output_buffer[1] == 9u)
                {
                    unique_exit_code = 32807;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text("\r\nInsufficient device capabilities." );
                    #endif
                }
                else if(spi_output_buffer[1] == 10u)
                {
                    unique_exit_code = 32809;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text("\r\nIncorrect DEVICEID." );
                    #endif
                }
                else if(spi_output_buffer[1] == 11u)
                {
                    unique_exit_code = 32811;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text("\r\nProgramming file is out of date, please regenerate.");
                    #endif
                }
                else if(spi_output_buffer[1] == 12u)
                {
                    unique_exit_code = 32813;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text("\r\nProgramming file does not support verification.");
                    #endif
                }
                else if(spi_output_buffer[1] == 13u)
                {
                    unique_exit_code = 32816;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text("\r\nInvalid or inaccessible Device Certificate.");
                    #endif
                }
                else if(spi_output_buffer[1] == 129u)
                {
                    unique_exit_code = 32797;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text("\r\nDevice security prevented operation.");
                    #endif
                }
                else if(spi_output_buffer[1] == 128u)
                {
                    if( ((spi_output_buffer[4] >> 2u) & 0x1fu) < 16u)
                    {
                        unique_exit_code = 32773;
                        #ifdef ENABLE_DISPLAY
                        dp_display_text("\r\nFailed to verify FPGA Array.");
                        #endif
                    }
                    else
                    {
                        unique_exit_code = 32774;
                        #ifdef ENABLE_DISPLAY
                        dp_display_text("\r\nFailed to verify Fabric Configuration.");
                        #endif
                    }
                }
                else if(spi_output_buffer[1] == 131u)
                {
                    tmp_buf = (spi_output_buffer[4] >> 2u) | (spi_output_buffer[5] << 6u);
                    if (((spi_output_buffer[4]& 0x3u) == 1u) && (tmp_buf >= 2u) && (tmp_buf <= 222u) && (((spi_output_buffer[6] >> 1u) & 0x3u) == 1u))
                    {
                        unique_exit_code = 32776;
                        #ifdef ENABLE_DISPLAY
                        dp_display_text("\r\nFailed to verify sNVM.");
                        #endif
                    }
                    else if (((spi_output_buffer[4]& 0x3u) == 1u) && (tmp_buf >= 2u) && (tmp_buf <= 222u) && (((spi_output_buffer[6] >> 1u) & 0x3u) == 2u))
                    {
                        unique_exit_code = 32857;
                        #ifdef ENABLE_DISPLAY
                        dp_display_text("\r\nFailed to verify pNVM.");
                        #endif
                    }
                    else if( (spi_output_buffer[4]& 0x3u) == 3u)
                    {
                        unique_exit_code = 32775;
                        #ifdef ENABLE_DISPLAY
                        dp_display_text("\r\nFailed to verify Security.");
                        #endif
                    }
                }
                #ifdef ENABLE_DISPLAY
                dp_display_text("\r\nERROR_CODE: ");
                dp_display_value(unique_exit_code, HEX);
                #endif
                g5_current_failed_component = global_uint2;
                g5_current_failed_block = global_ulong2;
                g5_current_unique_error_code = unique_exit_code;
                
                dp_G5M_read_debug_info();
                global_uint2 = global_uint1;
                break;
            }
            DataIndex += SPI_BUFFER_BIT_SIZE;
        }
        if ((Action_code == DP_PROGRAM_ACTION_CODE) && g5_componenet_Supports_Cert && (error_code == DPE_SUCCESS))
        {
            dp_G5M_read_component_digest();
            #ifdef ENABLE_DISPLAY
            if (g5_component_type == G5M_COMP_BITS)
                dp_display_text("\r\nBITS component bitstream digest: ");
            else if (g5_component_type == G5M_COMP_FPGA)
                dp_display_text("\r\nFabric component bitstream digest: ");
            else if (g5_component_type == G5M_COMP_KEYS)
                dp_display_text("\r\nSecurity component bitstream digest: ");
            else if (g5_component_type == G5M_COMP_SNVM)
                dp_display_text("\r\nsNVM component bitstream digest: ");
            else if (g5_component_type == G5M_COMP_OWP)
                dp_display_text("\r\neOWP component bitstream digest: ");
            else if (g5_component_type == G5M_COMP_ENVM)
                dp_display_text("\r\neNVM component bitstream digest: ");
            else if (g5_component_type == G5M_COMP_EOB)
                dp_display_text("\r\nEOB component bitstream digest: ");
            dp_display_array(g5_component_digest, G5M_COMPONENT_DIGEST_BYTE_SIZE, HEX);
            #endif
        }
    }
    
    return;
}

void dp_G5M_get_data_status(void)
{
    dp_flush_spi_buffers();
    spi_command_buffer = G5M_SPI_FRAME_STATUS;
    G5M_SPI_SCAN_out(0u,64u);
    
    if (spi_output_buffer[0] & 0x4u)
    {
        error_code = DPE_PROCESS_DATA_ERROR;
    }
    
    return;
}

void dp_G5M_read_component_digest(void)
{
    DPUINT index;
    dp_G5M_read_shared_buffer(G5M_NUMBER_OF_COFC_BLOCKS); // CofC is 928 bits which is 116 bytes which is 7.25 blocks of data
    
    for (index = 0;index < G5M_COMPONENT_DIGEST_BYTE_SIZE; index++)
    {
        // 20 is the byte location of the digest
        g5_component_digest[index] = G5M_shared_buf[20 + index];
    }
}

void dp_G5M_read_certificate(void)
{
    DPUCHAR cert_result;
    dp_flush_spi_buffers();
    
    spi_command_buffer = SPI_G5M_READ_DEVICE_CERT;
    G5M_SPI_SCAN_out(8, 8);
    
    cert_result = spi_output_buffer[0];
    
    if ((cert_result & 0x2u) == 0x2u)// error bit
    {  
        #ifdef ENABLE_DISPLAY
        dp_display_text("\r\nFailed to read device certificate.");
        #endif
        error_code = DPE_READ_CERTIFICATE_ERROR;
    }
    else if ((error_code == DPE_SUCCESS) && (cert_result & 0x1u == 0x0u))// valid bit
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text("\r\nDevice certificate could not be validated due to internal error.");
        #endif
        error_code = DPE_READ_CERTIFICATE_ERROR;
    }
    else
    {
        dp_G5M_read_shared_buffer(64u);
        #ifdef ENABLE_DISPLAY
        dp_display_text("\r\nDEVICE_CERTIFICATE(LSB->MSB): ");
        dp_display_array_reverse(G5M_shared_buf,1024, HEX);
        #endif
    }
}

void dp_G5M_query_security(void)
{
    dp_flush_spi_buffers();
    spi_command_buffer = G5M_SPI_QUERY_SECURITY;
    G5M_SPI_SCAN_in(0);
    if (device_family == G5SOC_DEVICE_FAMILY)
    {
        dp_G5M_read_shared_buffer(3u);
        #ifdef ENABLE_DISPLAY
        dp_display_text("\r\n--- Security locks and configuration settings ---\r\n");
        dp_display_array(G5M_shared_buf,33u,HEX);
        #endif
    }
    else
    {
        dp_G5M_read_shared_buffer(1u);
        #ifdef ENABLE_DISPLAY
        dp_display_text("\r\n--- Security locks and configuration settings ---\r\n");
        dp_display_array(G5M_shared_buf,9u,HEX);
        #endif
    }
    
    
    return;
}

#ifdef ENABLE_DISPLAY
void dp_G5M_dump_security(void)
{
    if ((G5M_shared_buf[7] & 0x10u) == 0x10u)
    {
        dp_display_text("\r\nSmartDebug user debug access and active probes are permanently disabled.");
        dp_display_text("\r\nSmartDebug sNVM debug is permanently disabled.");
        dp_display_text("\r\nSmartDebug Live probes are permanently disabled.");
        dp_display_text("\r\nReading of temperature and voltage via JTAG/SPI Slave is permanently disabled.");
    }
    else 
    {
        if ((G5M_shared_buf[0] & 0x1u) == 0x1u)
        {
            dp_display_text("\r\nSmartDebug user debug access and active probes are disabled.");
        }
        if ((G5M_shared_buf[0] & 0x2u) == 0x2u)
        {
            dp_display_text("\r\nSmartDebug sNVM debug is disabled.");
        }
        if ((G5M_shared_buf[0] & 0x4u) == 0x4u)
        {
            dp_display_text("\r\nSmartDebug Live probes are disabled.");
        }
        if ((G5M_shared_buf[0] & 0x8u) == 0x8u)
        {
            dp_display_text("\r\nUser JTAG interface is disabled");
        }
        if ((G5M_shared_buf[0] & 0x10u) == 0x10u)
        {
            dp_display_text("\r\nJTAG boundary scan is disabled.");
        }
        if ((G5M_shared_buf[0] & 0x20u) == 0x20u)
        {
            dp_display_text("\r\nReading of temperature and voltage via JTAG/SPI Slave is disabled.");
        }
    }
    if ((G5M_shared_buf[1] & 0x1u) == 0x1u)
    {
        dp_display_text("\r\nPlaintext passcode unlock is disabled.");
    }
    if ((G5M_shared_buf[1] & 0x2u) == 0x2u)
    {
        dp_display_text("\r\nFabric erase/write is permanently disabled.");
    }
    if ((G5M_shared_buf[7] & 0x20u) == 0x20u)
    {
        if ((G5M_shared_buf[1] & 0x2u) == 0x2u)
        {
            dp_display_text("\r\nFabric erase/write is disabled.");
        }
    }
    if ((G5M_shared_buf[1] & 0x4u) == 0x4u)
    {
        dp_display_text("\r\nExternal digest check via JTAG/SPI Slave is disabled.");
    }
    if ((G5M_shared_buf[1] & 0x8u) == 0x8u)
    {
        dp_display_text("\r\nBack level protection is enabled.");
    }
    if ((G5M_shared_buf[7] & 0x8u) == 0x8u)
    {
        dp_display_text("\r\nMicrosemi factory test access is permanently disabled.");
    }
    else
    {
        if ((G5M_shared_buf[1] & 0x10u) == 0x10u)
        {
            dp_display_text("\r\nMicrosemi factory test mode is disabled.");
        }
        else
        {
            dp_display_text("\r\nMicrosemi factory test mode access is allowed.");
        }
    }
    if ((G5M_shared_buf[1] & 0x20u) == 0x20u)
    {
        dp_display_text("\r\nAuto Programming and IAP Services are disabled.");
    }
    if ((G5M_shared_buf[1] & 0x40u) == 0x40u)
    {
        dp_display_text("\r\nExternal zeroizations via JTAG/SPI Slave is disabled.");
    }
    if ((G5M_shared_buf[1] & 0x80u) == 0x80u)
    {
        dp_display_text("\r\nSPI Slave port is disabled.");
    }
    if ((G5M_shared_buf[2] & 0x1u) == 0x1u)
    {
        dp_display_text("\r\nUser lock segment is locked. FlashLock/UPK1 is required to make changes to security.");
    }
    if ((G5M_shared_buf[2] & 0x2u) == 0x2u)
    {
        dp_display_text("\r\nAuthenticate programming action for JTAG/SPI Slave is disabled.");
    }
    if ((G5M_shared_buf[2] & 0x4u) == 0x4u)
    {
        dp_display_text("\r\nProgram action for JTAG/SPI Slave is disabled.");
    }
    if ((G5M_shared_buf[2] & 0x8u) == 0x8u)
    {
        dp_display_text("\r\nVerify action for JTAG/SPI Slave is disabled.");
    }
    if ((G5M_shared_buf[2] & 0x40u) == 0x40u)
    {
        dp_display_text("\r\nBitstream Default encryption key (KLK) is disabled.");
    }
    if ((G5M_shared_buf[2] & 0x80u) == 0x80u)
    {
        dp_display_text("\r\nBitstream User Encryption Key 1 is disabled.");
    }
    if ((G5M_shared_buf[3] & 0x1u) == 0x1u)
    {
        dp_display_text("\r\nBitstream User Encryption Key 2 is disabled.");
    }
    if ((G5M_shared_buf[3] & 0x4u) == 0x4u)
    {
        dp_display_text("\r\nBitstream AUTH_CODE key mode is disabled.");
    }
    if ((G5M_shared_buf[4] & 0x40u) == 0x40u)
    {
        dp_display_text("\r\nDefault encryption key (KLK) is disabled.");
    }
    if ((G5M_shared_buf[4] & 0x80u) == 0x80u)
    {
        dp_display_text("\r\nUser Encryption Key 1 is disabled.");
    }
    if ((G5M_shared_buf[5] & 0x1u) == 0x1u)
    {
        dp_display_text("\r\nUser Encryption Key 2 is disabled.");
    }
    if ((G5M_shared_buf[5] & 0x4u) == 0x4u)
    {
        dp_display_text("\r\nAUTH_CODE key mode is disabled.");
    }
    if ((G5M_shared_buf[6] & 0x10u) == 0x10u)
    {
        dp_display_text("\r\nsNVM write is disabled.");
    }
    if (device_family == G5SOC_DEVICE_FAMILY)
    {
        if ((G5M_shared_buf[7] & 0x1u) == 0x1u)
        {
            dp_display_text("\r\neNVM write is disabled.");
        }
    }
    if ((G5M_shared_buf[6] & 0x20u) == 0x20u)
    {
        dp_display_text("\r\nPUF emulation via JTAG/SPI Slave is disabled.");
    }
    if ((G5M_shared_buf[7] & 0x40u) == 0x40u)
    {
        dp_display_text("\r\nFlashLock/UPK1 unlocking is permanently disabled.");
        dp_display_text("\r\nUser lock segment is permanently locked.");
        dp_display_text("\r\nUser Key Set 1 is permanently locked.");
    }
    else
    {
        if ((G5M_shared_buf[7] & 0x2u) == 0x2u)// Bit 57
        {
            dp_display_text("\r\nUser Key Set 1 is locked. FlashLock/UPK1 is required to make changes.");
        }
        if ((G5M_shared_buf[2] & 0x1u) == 0x1u)// Bit 16
        {
            dp_display_text("\r\nUser lock segment is locked. FlashLock/UPK1 is required to make changes to security.");
        }
    }
    if ((G5M_shared_buf[7] & 0x80u) == 0x80u)// Bit 63
    {
        dp_display_text("\r\nFlashLock/UPK2 unlocking is permanently disabled.");
    }
    else
    {
        if ((G5M_shared_buf[7] & 0x4u) == 0x4u)// Bit 58
        {
            dp_display_text("\r\nUser Key Set 2 is locked. FlashLock/UPK2 is required to make changes.");
        }
    }
    if ((G5M_shared_buf[8] & 0x1u) == 0x1u)// Bit 64
    {
        dp_display_text("\r\nFlashLock/DPK unlocking is permanently disabled.");
    }
    if ((G5M_shared_buf[8] & 0x2u) == 0x2u)// Bit 65
    {
        dp_display_text("\r\nPermanent lock segment is permanently locked.");
    }
    if (device_family == G5SOC_DEVICE_FAMILY)
    {
        if ((G5M_shared_buf[12] & 0x1u) == 0x1u)// Bit 96
        {
            dp_display_text("\r\nUser crypto core is enabled.");
            if ((G5M_shared_buf[12] & 0x6u) == 0x0u)// Bits 97, 98
            {
                dp_display_text("\r\nUser crypto core mode is MSS");
            }
            else if ((G5M_shared_buf[12] & 0x6u) == 0x2u)
            {
                dp_display_text("\r\nUser crypto core mode is FABRIC.");
            }
            else if ((G5M_shared_buf[12] & 0x6u) == 0x4u)
            {
                dp_display_text("\r\nUser crypto core mode is SHARED_MSS.");
            }
            else if ((G5M_shared_buf[12] & 0x6u) == 0x6u)
            {
                dp_display_text("\r\nUser crypto core mode is SHARED_FABRIC.");
            }
        }
    }
    if ((G5M_shared_buf[14] & 0x1u) == 0x1u)// Bit 112
    {
        dp_display_text("\r\nPower on digest is enabled for Fabric.");
    }
    if ((G5M_shared_buf[14] & 0x2u) == 0x2u)// Bit 113
    {
        dp_display_text("\r\nPower on digest is enabled for Fabric Configuration.");
    }
    if ((G5M_shared_buf[14] & 0x4u) == 0x4u)// Bit 114
    {
        dp_display_text("\r\nPower on digest is enabled for sNVM.");
    }
    if ((G5M_shared_buf[14] & 0x8u) == 0x8u)// Bit 115
    {
        dp_display_text("\r\nPower on digest is enabled for User security policies.");
    }
    if ((G5M_shared_buf[14] & 0x10u) == 0x10u)// Bit 116
    {
        dp_display_text("\r\nPower on digest is enabled for UKDIGEST0 in User Key segment.");
    }
    if ((G5M_shared_buf[14] & 0x20u) == 0x20u)// Bit 117
    {
        dp_display_text("\r\nPower on digest is enabled for UKDIGEST1 in User Key segment.");
    }
    if ((G5M_shared_buf[14] & 0x40u) == 0x40u)// Bit 118
    {
        dp_display_text("\r\nPower on digest is enabled for UPK1.");
    }
    if ((G5M_shared_buf[14] & 0x80u) == 0x80u)// Bit 119
    {
        dp_display_text("\r\nPower on digest is enabled for UEK1.");
    }
    if ((G5M_shared_buf[15] & 0x1u) == 0x1u)// Bit 120
    {
        dp_display_text("\r\nPower on digest is enabled for DPK.");
    }
    if ((G5M_shared_buf[15] & 0x2u) == 0x2u)// Bit 121
    {
        dp_display_text("\r\nPower on digest is enabled for UPK2.");
    }
    if ((G5M_shared_buf[15] & 0x4u) == 0x4u)// Bit 122
    {
        dp_display_text("\r\nPower on digest is enabled for UEK2.");
    }
    if ((G5M_shared_buf[15] & 0x8u) == 0x8u)// Bit 123
    {
        dp_display_text("\r\nPower on digest is enabled for Permanent lock settings.");
    }
    if ((G5M_shared_buf[15] & 0x10u) == 0x10u)// Bit 124
    {
        dp_display_text("\r\nPower on digest is enabled for Factory row and factory key.");
    }
    if (device_family == G5SOC_DEVICE_FAMILY)
    {
        if ((G5M_shared_buf[15] & 0x40u) == 0x40u)// Bit 126
        {
            dp_display_text("\r\nPower on digest is enabled for eNVM.");
        }
        if ((G5M_shared_buf[15] & 0x80u) == 0x80u)// Bit 127
        {
            dp_display_text("\r\nPower on digest is enabled for MSS Secure Boot info.");
        }
        if ((G5M_shared_buf[32] & 0x1u) == 0x1u)// Bit 256
        {
            dp_display_text("\r\nPower on digest is enabled for sNVM read/write access info.");
        }
        if ((G5M_shared_buf[32] & 0x2u) == 0x2u)// Bit 257
        {
            dp_display_text("\r\nPower on digest is enabled for SBIC revocation.");
        }
    }
    return;
}
#endif


void dp_G5M_unlock_upk1(void)
{
    dp_get_data(GXM_UPK1_ID,0u);
    if (return_bytes == 0u)
    {
        #ifdef ENABLE_DISPLAY      
        dp_display_text("\r\nWarning: UPK1 data is missing.");
        #endif
    }
    else
    {
        dp_G5M_load_upk1();
        if (error_code == DPE_SUCCESS)
        {
            dp_flush_spi_buffers();
            spi_command_buffer = G5M_UNLOCK_USER_PASSCODE1;
            G5M_SPI_SCAN_out(0u,8u);
        }
        if ((error_code != DPE_SUCCESS) || ((spi_output_buffer[0] & 0x3u) != 0x1u) )
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text("\r\nFailed to unlock user pass key 1.");
            #endif
            error_code = DPE_MATCH_ERROR;
        }
        #ifdef ENABLE_DISPLAY
        else
        {
            dp_display_text("\r\nUser security (DPK1) is unlocked.");
        }
        #endif
    }
    return;
}

void dp_G5M_unlock_upk2(void)
{
    dp_get_data(GXM_UPK2_ID,0u);
    if (return_bytes == 0u)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text("\r\nWarning: UPK2 data is missing.");
        #endif
    }
    else
    {
        dp_G5M_load_upk2();
        if (error_code == DPE_SUCCESS)
        {
            dp_flush_spi_buffers();
            spi_command_buffer = G5M_UNLOCK_USER_PASSCODE2;
            G5M_SPI_SCAN_out(0u,8u);
        }
        if ((error_code != DPE_SUCCESS) || ((spi_output_buffer[0] & 0x3u) != 0x1u) )
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text("\r\nFailed to unlock user pass key 2.");
            #endif
            error_code = DPE_MATCH_ERROR;
        }
        #ifdef ENABLE_DISPLAY
        else
        {
            dp_display_text("\r\nUser security (DPK2) is unlocked.");
        }
        #endif
    }
    return;
}

void dp_G5M_load_upk1(void)
{
    spi_command_buffer = G5M_SPI_LEY_LO;
    dp_G5M_get_and_shift_in(GXM_UPK1_ID, SPI_BUFFER_BIT_SIZE, 0u);
    spi_command_buffer = G5M_SPI_LEY_HI;
    dp_G5M_get_and_shift_in(GXM_UPK1_ID, SPI_BUFFER_BIT_SIZE, SPI_BUFFER_BIT_SIZE);
    
    return;
}

void dp_G5M_load_upk2(void)
{
    spi_command_buffer = G5M_SPI_LEY_LO;
    dp_G5M_get_and_shift_in(GXM_UPK2_ID, SPI_BUFFER_BIT_SIZE, 0u);
    spi_command_buffer = G5M_SPI_LEY_HI;
    dp_G5M_get_and_shift_in(GXM_UPK2_ID, SPI_BUFFER_BIT_SIZE, SPI_BUFFER_BIT_SIZE);
    
    return;
}

void dp_G5M_display_bitstream_digest(void)
{
    
    DataIndex = 0u;
    global_uint1 = (DPUINT)dp_get_bytes(Header_ID,GXM_DATASIZE_OFFSET,GXM_DATASIZE_BYTE_LENGTH);
    for (global_uint2 = 1u; global_uint2 <= global_uint1; global_uint2++)
    {
        /* get the number of blocks */
        /* Global ulong1 is used to hold the number of blocks within the components */
        global_ulong1 = dp_get_bytes(GXM_NUMBER_OF_BLOCKS_ID,(DPULONG)(((global_uint2 - 1u) * 22u) / 8u),4u);
        global_ulong1 >>= ((global_uint2 - 1U)* 22u) % 8u;
        global_ulong1 &= 0x3FFFFFu;
        
        g5_component_type = (DPUCHAR) dp_get_bytes(GXM_datastream_ID,G5M_COMPONENT_TYPE_IN_HEADER_BYTE+DataIndex/8,1);
        #ifdef ENABLE_DISPLAY
        if (g5_component_type == G5M_COMP_BITS)
        {
            DPUCHAR * data_address = (DPUCHAR*)DPNULL;
            data_address = dp_get_data(GXM_datastream_ID, G5M_BSDIGEST_BYTE_OFFSET * 8 + DataIndex);
            dp_display_text("\r\nBITSTREAM_DIGEST = ");
            dp_display_array(data_address, G5M_BSDIGEST_BYTE_SIZE, HEX);
        }
        #endif
        DataIndex += SPI_BUFFER_BIT_SIZE * global_ulong1;
    }
    
    
    return;
}

void dp_G5M_do_zeroize(DPUCHAR zmode)
{
    DPUCHAR zeroize_result[16] = {0x00, 0xB6, 0x16, 0x3B, 0x25, 0xC3, 0x0A, 0xE5, 0x7B, 0x5D, 0x19, 0x00, 0x45, 0x06, 0x31, 0xA8};
    DPUCHAR index;
    
    zeroize_result[0] = zmode;
    
    dp_flush_spi_buffers();
    
    for (index = 0; index < 16; index++)
        spi_input_buffer[index] = zeroize_result[index];
    
    spi_command_buffer = G5M_ZEROIZE;
    G5M_SPI_SCAN_out(SPI_BUFFER_BIT_SIZE,SPI_BUFFER_BIT_SIZE);
    
    if(spi_output_buffer[0] & 0x1u == 1u)
    {
        unique_exit_code = 32849;
        #ifdef ENABLE_DISPLAY
        dp_display_text("\r\nZEROIZE_RESULT: ");
        dp_display_array(spi_output_buffer, 16, HEX);
        error_code = DPE_POLL_ERROR;
        dp_display_text("\r\nFailed to zeroize the device.\r\nERROR_CODE: ");
        dp_display_value(unique_exit_code, HEX);
        #endif
    }
    else
    {
        dp_G5M_do_read_zeroization_result();
    }
    
    return;
}

void dp_G5M_do_read_zeroization_result(void)
{
    dp_flush_spi_buffers();
    spi_command_buffer = G5M_READ_ZEROIZATION_RESULT;
    G5M_SPI_SCAN_out(SPI_BUFFER_BIT_SIZE,SPI_BUFFER_BIT_SIZE);
    
    if(spi_output_buffer[0] & 0x3u > 0u)
    {
        error_code = DPE_POLL_ERROR;
        unique_exit_code = 32854;
        #ifdef ENABLE_DISPLAY
        dp_display_text("\r\nread_zeroize_result: ");
        dp_display_array(spi_output_buffer, 16, HEX);
        dp_display_text("\r\nFailed to read zeroization certificate.\r\nERROR_CODE: ");
        dp_display_value(unique_exit_code, HEX);
        #endif
    }
    else
    {
        dp_G5M_read_shared_buffer(9u);
        #ifdef ENABLE_DISPLAY      
        dp_display_text("\r\nFETCH_ZEROIZATION_RESULT: ");
        dp_display_array(G5M_shared_buf, 131, HEX);
        #endif      
    }
    
    return;
}

#endif

/*   *************** End of File *************** */

