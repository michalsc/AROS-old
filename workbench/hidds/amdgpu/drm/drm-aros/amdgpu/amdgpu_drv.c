
#include "drmP.h"

#include "drm_compat_types.h"
#include "amdgpu_drv.h"

int amdgpu_vram_limit = 0;
int amdgpu_gart_size = -1; /* auto */
int amdgpu_moverate = -1;  /* auto */
int amdgpu_benchmarking = 0;
int amdgpu_testing = 0;
int amdgpu_audio = -1;
int amdgpu_disp_priority = 0;
int amdgpu_hw_i2c = 0;
int amdgpu_pcie_gen2 = -1;
int amdgpu_msi = -1;
int amdgpu_lockup_timeout = 0;
int amdgpu_dpm = -1;
int amdgpu_fw_load_type = -1;
int amdgpu_aspm = -1;
int amdgpu_runtime_pm = -1;
unsigned amdgpu_ip_block_mask = 0xffffffff;
int amdgpu_bapm = -1;
int amdgpu_deep_color = 0;
int amdgpu_vm_size = -1;
int amdgpu_vm_block_size = -1;
int amdgpu_vm_fault_stop = 0;
int amdgpu_vm_debug = 0;
int amdgpu_vram_page_split = 512;
int amdgpu_vm_update_mode = -1;
int amdgpu_exp_hw_support = 0;
int amdgpu_sched_jobs = 32;
int amdgpu_sched_hw_submission = 2;
int amdgpu_no_evict = 0;
int amdgpu_direct_gma_size = 0;
unsigned amdgpu_pcie_gen_cap = 0;
unsigned amdgpu_pcie_lane_cap = 0;
unsigned amdgpu_cg_mask = 0xffffffff;
unsigned amdgpu_pg_mask = 0xffffffff;
char *amdgpu_disable_cu = NULL;
char *amdgpu_virtual_display = NULL;
unsigned amdgpu_pp_feature_mask = 0xffffffff;
int amdgpu_ngg = 0;
int amdgpu_prim_buf_per_se = 0;
int amdgpu_pos_buf_per_se = 0;
int amdgpu_cntl_sb_buf_per_se = 0;
int amdgpu_param_buf_per_se = 0;
int amdgpu_job_hang_limit = 0;
int amdgpu_lbpw = -1;

#ifdef CONFIG_DRM_AMDGPU_SI
#if defined(CONFIG_DRM_RADEON) || defined(CONFIG_DRM_RADEON_MODULE)
int amdgpu_si_support = 0;
#else
int amdgpu_si_support = 1;
#endif
#endif

#ifdef CONFIG_DRM_AMDGPU_CIK
#if defined(CONFIG_DRM_RADEON) || defined(CONFIG_DRM_RADEON_MODULE)
int amdgpu_cik_support = 0;
#else
int amdgpu_cik_support = 1;
#endif
#endif

static const struct pci_device_id pciidlist[] = {
#ifdef CONFIG_DRM_AMDGPU_SI
    {0x1002, 0x6780, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TAHITI},
    {0x1002, 0x6784, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TAHITI},
    {0x1002, 0x6788, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TAHITI},
    {0x1002, 0x678A, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TAHITI},
    {0x1002, 0x6790, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TAHITI},
    {0x1002, 0x6791, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TAHITI},
    {0x1002, 0x6792, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TAHITI},
    {0x1002, 0x6798, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TAHITI},
    {0x1002, 0x6799, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TAHITI},
    {0x1002, 0x679A, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TAHITI},
    {0x1002, 0x679B, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TAHITI},
    {0x1002, 0x679E, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TAHITI},
    {0x1002, 0x679F, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TAHITI},
    {0x1002, 0x6800, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_PITCAIRN | AMD_IS_MOBILITY},
    {0x1002, 0x6801, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_PITCAIRN | AMD_IS_MOBILITY},
    {0x1002, 0x6802, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_PITCAIRN | AMD_IS_MOBILITY},
    {0x1002, 0x6806, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_PITCAIRN},
    {0x1002, 0x6808, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_PITCAIRN},
    {0x1002, 0x6809, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_PITCAIRN},
    {0x1002, 0x6810, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_PITCAIRN},
    {0x1002, 0x6811, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_PITCAIRN},
    {0x1002, 0x6816, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_PITCAIRN},
    {0x1002, 0x6817, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_PITCAIRN},
    {0x1002, 0x6818, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_PITCAIRN},
    {0x1002, 0x6819, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_PITCAIRN},
    {0x1002, 0x6600, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND | AMD_IS_MOBILITY},
    {0x1002, 0x6601, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND | AMD_IS_MOBILITY},
    {0x1002, 0x6602, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND | AMD_IS_MOBILITY},
    {0x1002, 0x6603, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND | AMD_IS_MOBILITY},
    {0x1002, 0x6604, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND | AMD_IS_MOBILITY},
    {0x1002, 0x6605, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND | AMD_IS_MOBILITY},
    {0x1002, 0x6606, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND | AMD_IS_MOBILITY},
    {0x1002, 0x6607, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND | AMD_IS_MOBILITY},
    {0x1002, 0x6608, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND},
    {0x1002, 0x6610, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND},
    {0x1002, 0x6611, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND},
    {0x1002, 0x6613, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND},
    {0x1002, 0x6617, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND | AMD_IS_MOBILITY},
    {0x1002, 0x6620, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND | AMD_IS_MOBILITY},
    {0x1002, 0x6621, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND | AMD_IS_MOBILITY},
    {0x1002, 0x6623, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND | AMD_IS_MOBILITY},
    {0x1002, 0x6631, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_OLAND},
    {0x1002, 0x6820, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE | AMD_IS_MOBILITY},
    {0x1002, 0x6821, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE | AMD_IS_MOBILITY},
    {0x1002, 0x6822, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE | AMD_IS_MOBILITY},
    {0x1002, 0x6823, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE | AMD_IS_MOBILITY},
    {0x1002, 0x6824, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE | AMD_IS_MOBILITY},
    {0x1002, 0x6825, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE | AMD_IS_MOBILITY},
    {0x1002, 0x6826, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE | AMD_IS_MOBILITY},
    {0x1002, 0x6827, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE | AMD_IS_MOBILITY},
    {0x1002, 0x6828, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE},
    {0x1002, 0x6829, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE},
    {0x1002, 0x682A, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE | AMD_IS_MOBILITY},
    {0x1002, 0x682B, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE | AMD_IS_MOBILITY},
    {0x1002, 0x682C, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE},
    {0x1002, 0x682D, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE | AMD_IS_MOBILITY},
    {0x1002, 0x682F, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE | AMD_IS_MOBILITY},
    {0x1002, 0x6830, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE | AMD_IS_MOBILITY},
    {0x1002, 0x6831, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE | AMD_IS_MOBILITY},
    {0x1002, 0x6835, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE},
    {0x1002, 0x6837, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE},
    {0x1002, 0x6838, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE},
    {0x1002, 0x6839, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE},
    {0x1002, 0x683B, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE},
    {0x1002, 0x683D, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE},
    {0x1002, 0x683F, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VERDE},
    {0x1002, 0x6660, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAINAN | AMD_IS_MOBILITY},
    {0x1002, 0x6663, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAINAN | AMD_IS_MOBILITY},
    {0x1002, 0x6664, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAINAN | AMD_IS_MOBILITY},
    {0x1002, 0x6665, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAINAN | AMD_IS_MOBILITY},
    {0x1002, 0x6667, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAINAN | AMD_IS_MOBILITY},
    {0x1002, 0x666F, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAINAN | AMD_IS_MOBILITY},
#endif
#ifdef CONFIG_DRM_AMDGPU_CIK
    /* Kaveri */
    {0x1002, 0x1304, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x1305, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_APU},
    {0x1002, 0x1306, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x1307, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_APU},
    {0x1002, 0x1309, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x130A, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x130B, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x130C, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x130D, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x130E, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x130F, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_APU},
    {0x1002, 0x1310, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_APU},
    {0x1002, 0x1311, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_APU},
    {0x1002, 0x1312, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_APU},
    {0x1002, 0x1313, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_APU},
    {0x1002, 0x1315, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_APU},
    {0x1002, 0x1316, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_APU},
    {0x1002, 0x1317, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x1318, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x131B, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_APU},
    {0x1002, 0x131C, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_APU},
    {0x1002, 0x131D, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KAVERI | AMD_IS_APU},
    /* Bonaire */
    {0x1002, 0x6640, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_BONAIRE | AMD_IS_MOBILITY},
    {0x1002, 0x6641, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_BONAIRE | AMD_IS_MOBILITY},
    {0x1002, 0x6646, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_BONAIRE | AMD_IS_MOBILITY},
    {0x1002, 0x6647, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_BONAIRE | AMD_IS_MOBILITY},
    {0x1002, 0x6649, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_BONAIRE},
    {0x1002, 0x6650, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_BONAIRE},
    {0x1002, 0x6651, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_BONAIRE},
    {0x1002, 0x6658, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_BONAIRE},
    {0x1002, 0x665c, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_BONAIRE},
    {0x1002, 0x665d, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_BONAIRE},
    {0x1002, 0x665f, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_BONAIRE},
    /* Hawaii */
    {0x1002, 0x67A0, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAWAII},
    {0x1002, 0x67A1, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAWAII},
    {0x1002, 0x67A2, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAWAII},
    {0x1002, 0x67A8, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAWAII},
    {0x1002, 0x67A9, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAWAII},
    {0x1002, 0x67AA, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAWAII},
    {0x1002, 0x67B0, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAWAII},
    {0x1002, 0x67B1, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAWAII},
    {0x1002, 0x67B8, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAWAII},
    {0x1002, 0x67B9, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAWAII},
    {0x1002, 0x67BA, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAWAII},
    {0x1002, 0x67BE, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_HAWAII},
    /* Kabini */
    {0x1002, 0x9830, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x9831, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_APU},
    {0x1002, 0x9832, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x9833, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_APU},
    {0x1002, 0x9834, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x9835, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_APU},
    {0x1002, 0x9836, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x9837, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_APU},
    {0x1002, 0x9838, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x9839, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x983a, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_APU},
    {0x1002, 0x983b, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x983c, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_APU},
    {0x1002, 0x983d, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_APU},
    {0x1002, 0x983e, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_APU},
    {0x1002, 0x983f, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_KABINI | AMD_IS_APU},
    /* mullins */
    {0x1002, 0x9850, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x9851, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x9852, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x9853, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x9854, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x9855, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x9856, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x9857, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x9858, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x9859, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x985A, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x985B, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x985C, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x985D, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x985E, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
    {0x1002, 0x985F, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_MULLINS | AMD_IS_MOBILITY | AMD_IS_APU},
#endif
    /* topaz */
    {0x1002, 0x6900, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TOPAZ},
    {0x1002, 0x6901, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TOPAZ},
    {0x1002, 0x6902, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TOPAZ},
    {0x1002, 0x6903, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TOPAZ},
    {0x1002, 0x6907, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TOPAZ},
    /* tonga */
    {0x1002, 0x6920, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TONGA},
    {0x1002, 0x6921, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TONGA},
    {0x1002, 0x6928, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TONGA},
    {0x1002, 0x6929, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TONGA},
    {0x1002, 0x692B, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TONGA},
    {0x1002, 0x692F, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TONGA},
    {0x1002, 0x6930, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TONGA},
    {0x1002, 0x6938, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TONGA},
    {0x1002, 0x6939, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_TONGA},
    /* fiji */
    {0x1002, 0x7300, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_FIJI},
    {0x1002, 0x730F, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_FIJI},
    /* carrizo */
    {0x1002, 0x9870, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_CARRIZO | AMD_IS_APU},
    {0x1002, 0x9874, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_CARRIZO | AMD_IS_APU},
    {0x1002, 0x9875, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_CARRIZO | AMD_IS_APU},
    {0x1002, 0x9876, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_CARRIZO | AMD_IS_APU},
    {0x1002, 0x9877, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_CARRIZO | AMD_IS_APU},
    /* stoney */
    {0x1002, 0x98E4, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_STONEY | AMD_IS_APU},
    /* Polaris11 */
    {0x1002, 0x67E0, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS11},
    {0x1002, 0x67E3, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS11},
    {0x1002, 0x67E8, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS11},
    {0x1002, 0x67EB, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS11},
    {0x1002, 0x67EF, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS11},
    {0x1002, 0x67FF, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS11},
    {0x1002, 0x67E1, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS11},
    {0x1002, 0x67E7, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS11},
    {0x1002, 0x67E9, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS11},
    /* Polaris10 */
    {0x1002, 0x67C0, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS10},
    {0x1002, 0x67C1, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS10},
    {0x1002, 0x67C2, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS10},
    {0x1002, 0x67C4, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS10},
    {0x1002, 0x67C7, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS10},
    {0x1002, 0x67D0, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS10},
    {0x1002, 0x67DF, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS10},
    {0x1002, 0x67C8, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS10},
    {0x1002, 0x67C9, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS10},
    {0x1002, 0x67CA, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS10},
    {0x1002, 0x67CC, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS10},
    {0x1002, 0x67CF, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS10},
    /* Polaris12 */
    {0x1002, 0x6980, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS12},
    {0x1002, 0x6981, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS12},
    {0x1002, 0x6985, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS12},
    {0x1002, 0x6986, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS12},
    {0x1002, 0x6987, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS12},
    {0x1002, 0x6995, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS12},
    {0x1002, 0x6997, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS12},
    {0x1002, 0x699F, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_POLARIS12},
    /* Vega 10 */
    {0x1002, 0x6860, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VEGA10 | AMD_EXP_HW_SUPPORT},
    {0x1002, 0x6861, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VEGA10 | AMD_EXP_HW_SUPPORT},
    {0x1002, 0x6862, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VEGA10 | AMD_EXP_HW_SUPPORT},
    {0x1002, 0x6863, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VEGA10 | AMD_EXP_HW_SUPPORT},
    {0x1002, 0x6864, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VEGA10 | AMD_EXP_HW_SUPPORT},
    {0x1002, 0x6867, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VEGA10 | AMD_EXP_HW_SUPPORT},
    {0x1002, 0x6868, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VEGA10 | AMD_EXP_HW_SUPPORT},
    {0x1002, 0x686c, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VEGA10 | AMD_EXP_HW_SUPPORT},
    {0x1002, 0x687f, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_VEGA10 | AMD_EXP_HW_SUPPORT},
    /* Raven */
    {0x1002, 0x15dd, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CHIP_RAVEN | AMD_IS_APU | AMD_EXP_HW_SUPPORT},

    {0, 0, 0}
};

static struct drm_driver kms_driver = {
    .driver_features =
        DRIVER_USE_AGP |
        DRIVER_HAVE_IRQ | DRIVER_IRQ_SHARED | DRIVER_GEM |
        DRIVER_PRIME | DRIVER_RENDER | DRIVER_MODESET | DRIVER_SYNCOBJ,
#if 0
    .load = amdgpu_driver_load_kms,
    .open = amdgpu_driver_open_kms,
    .postclose = amdgpu_driver_postclose_kms,
    .lastclose = amdgpu_driver_lastclose_kms,
    .set_busid = drm_pci_set_busid,
    .unload = amdgpu_driver_unload_kms,
    .get_vblank_counter = amdgpu_get_vblank_counter_kms,
    .enable_vblank = amdgpu_enable_vblank_kms,
    .disable_vblank = amdgpu_disable_vblank_kms,
    .get_vblank_timestamp = drm_calc_vbltimestamp_from_scanoutpos,
    .get_scanout_position = amdgpu_get_crtc_scanout_position,
#if defined(CONFIG_DEBUG_FS)
    .debugfs_init = amdgpu_debugfs_init,
#endif
    .irq_preinstall = amdgpu_irq_preinstall,
    .irq_postinstall = amdgpu_irq_postinstall,
    .irq_uninstall = amdgpu_irq_uninstall,
    .irq_handler = amdgpu_irq_handler,
    .ioctls = amdgpu_ioctls_kms,
    .gem_free_object_unlocked = amdgpu_gem_object_free,
    .gem_open_object = amdgpu_gem_object_open,
    .gem_close_object = amdgpu_gem_object_close,
    .dumb_create = amdgpu_mode_dumb_create,
    .dumb_map_offset = amdgpu_mode_dumb_mmap,
    .dumb_destroy = drm_gem_dumb_destroy,
    .fops = &amdgpu_driver_kms_fops,
    .prime_handle_to_fd = drm_gem_prime_handle_to_fd,
    .prime_fd_to_handle = drm_gem_prime_fd_to_handle,
    .gem_prime_export = amdgpu_gem_prime_export,
    .gem_prime_import = drm_gem_prime_import,
    .gem_prime_pin = amdgpu_gem_prime_pin,
    .gem_prime_unpin = amdgpu_gem_prime_unpin,
    .gem_prime_res_obj = amdgpu_gem_prime_res_obj,
    .gem_prime_get_sg_table = amdgpu_gem_prime_get_sg_table,
    .gem_prime_import_sg_table = amdgpu_gem_prime_import_sg_table,
    .gem_prime_vmap = amdgpu_gem_prime_vmap,
    .gem_prime_vunmap = amdgpu_gem_prime_vunmap,

    .name = DRIVER_NAME,
    .desc = DRIVER_DESC,
    .date = DRIVER_DATE,
    .major = KMS_DRIVER_MAJOR,
    .minor = KMS_DRIVER_MINOR,
    .patchlevel = KMS_DRIVER_PATCHLEVEL,
#endif
};

static int amdgpu_pci_probe(struct pci_dev *pdev,
                            const struct pci_device_id *ent)
{
    struct drm_device *dev;
    unsigned long flags = ent->driver_data;
//    int ret;

    if ((flags & AMD_EXP_HW_SUPPORT) && !amdgpu_exp_hw_support)
    {
        DRM_INFO("This hardware requires experimental hardware support.\n"
                 "See modparam exp_hw_support\n");
        return -ENODEV;
    }
#if 0 // Not necessary yet???
    /*
         * Initialize amdkfd before starting radeon. If it was not loaded yet,
         * defer radeon probing
         */
    ret = amdgpu_amdkfd_init();
    if (ret == -EPROBE_DEFER)
        return ret;
#endif
#if 0 // Not necessary yet???
    /* Get rid of things like offb */
    ret = amdgpu_kick_out_firmware_fb(pdev);
    if (ret)
        return ret;
#endif

    dev = drm_dev_alloc(&kms_driver, &pdev->dev);
    if (IS_ERR(dev))
        return PTR_ERR(dev);
#if 0
    ret = pci_enable_device(pdev);
    if (ret)
        goto err_free;

    dev->pdev = pdev;

    pci_set_drvdata(pdev, dev);

    ret = drm_dev_register(dev, ent->driver_data);
    if (ret)
        goto err_pci;
#endif
    return 0;
#if 0
err_pci:
    pci_disable_device(pdev);
err_free:
    drm_dev_unref(dev);
    return ret;
    #endif
}

static struct drm_driver *driver;
static struct pci_driver *pdriver;

static struct pci_driver amdgpu_kms_pci_driver = {
    .name = DRIVER_NAME,
    .id_table = pciidlist,
    .probe = amdgpu_pci_probe,
    //    .remove = amdgpu_pci_remove,
    //    .shutdown = amdgpu_pci_shutdown,
    //    .driver.pm = &amdgpu_pm_ops,
};

int amdgpu_init()
{
    DRM_INFO("amdgpu_init()\n");

    driver = &kms_driver;
    pdriver = &amdgpu_kms_pci_driver;
    pdriver->driver = driver;

    drm_aros_pci_init(driver);

    return pci_register_driver(pdriver);
}