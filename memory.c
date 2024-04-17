#include "memory.h"

//static bool flash_ready = true;
static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt);

NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
{
    .evt_handler = fstorage_evt_handler
};


void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage)
{
    while (nrf_fstorage_is_busy(p_fstorage))
    {
        (void) sd_app_evt_wait();
    }
}


static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt)
{
    if (p_evt->result != NRF_SUCCESS)
    {
        NRF_LOG_INFO("--> Event received: ERROR while executing an fstorage operation.");
        return;
    }
    switch (p_evt->id)
    {
        case NRF_FSTORAGE_EVT_WRITE_RESULT:
        {
            uint32_t * data = (uint32_t *)(p_evt->addr);
            NRF_LOG_INFO("--> Event received: wrote %d bytes at address 0x%x, %d",
                         p_evt->len, p_evt->addr, *(data))
            //flash_ready = true;
        } break;

        case NRF_FSTORAGE_EVT_ERASE_RESULT:
        {
            uint32_t * data = (uint32_t *)(p_evt->addr);
            NRF_LOG_INFO("--> Event received: erased %d page from address 0x%x, %d",
                         p_evt->len, p_evt->addr, *(data));
            //flash_ready = true;
        } break;

        default:
            break;
    }
}


static uint32_t nrf5_flash_end_addr_get()
{
    uint32_t const bootloader_addr = NRF_UICR->NRFFW[0];
    uint32_t const page_sz         = NRF_FICR->CODEPAGESIZE;
    uint32_t const code_sz         = NRF_FICR->CODESIZE;

    return (bootloader_addr != 0xFFFFFFFF ?
            bootloader_addr : (code_sz * page_sz));
}


static uint32_t round_up_u32(uint32_t len)
{
    if (len % sizeof(uint32_t))
    {
        return (len + sizeof(uint32_t) - (len % sizeof(uint32_t)));
    }

    return len;
}


static void fstorage_read(uint32_t addr, uint8_t * buffer, uint32_t len)
{
    ret_code_t  rc;
    rc = nrf_fstorage_read(&fstorage, addr, buffer, len);
    if (rc != NRF_SUCCESS)
    {
        NRF_LOG_INFO("nrf_fstorage_read() returned: %s", nrf_strerror_get(rc));
        return;
        
    }
    NRF_LOG_INFO("reading address %x:",addr);
    NRF_LOG_HEXDUMP_INFO(buffer, len);      
}


static void fstorage_write(uint32_t addr, void const * p_data, uint32_t len)
{
    NRF_LOG_INFO("BLOCK");
    //flash_ready = false;
    len = round_up_u32(len);
    ret_code_t rc = nrf_fstorage_write(&fstorage, addr, p_data, len, NULL);
    if (rc != NRF_SUCCESS)
    {
        NRF_LOG_INFO("nrf_fstorage_write() returned: %s", nrf_strerror_get(rc));
    }
}


static void fstorage_erase(uint32_t addr, uint32_t pages_cnt)
{
    NRF_LOG_INFO("BLOCK");
    //flash_ready = false;
    ret_code_t rc = nrf_fstorage_erase(&fstorage, addr, pages_cnt, NULL);
    if (rc != NRF_SUCCESS)
    {
        NRF_LOG_INFO("nrf_fstorage_erase() returned: %s\r\n", nrf_strerror_get(rc));
    }
}


void fstorage_init()
{
    ret_code_t rc;
    nrf_fstorage_api_t * p_fs_api;
    p_fs_api = &nrf_fstorage_sd;
    uint32_t end_addr = nrf5_flash_end_addr_get();
    fstorage.end_addr = end_addr - 1;
    uint32_t start_addr = end_addr - 0x02000;
    fstorage.start_addr = start_addr;
    rc = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);
}


void save_led(uint8_t * arr, int len)
{
    NRF_LOG_INFO("Start addr: 0x%x", fstorage.start_addr)
    NRF_LOG_INFO("End addr: 0x%x", fstorage.end_addr)
    fstorage_erase(fstorage.start_addr, 1);
    //NRF_LOG_INFO("WAIT");
    //while(!flash_ready) {}
    //while (nrf_fstorage_is_busy(&fstorage)) {}
    fstorage_write(fstorage.start_addr, arr, len);
    //while (nrf_fstorage_is_busy(&fstorage)) {}
    //NRF_LOG_INFO("WAIT");
    //while(!flash_ready) {}
}


void get_led(uint8_t * arr, int len)
{
    fstorage_read(fstorage.start_addr, arr, len);
}