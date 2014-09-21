#===========================================================================
#
# MODULE              JN-AN-1162 JenNet-IP Smart Home
#
# DESCRIPTION         MibCommon Library Patch Points
#
#===========================================================================
#
# This software is owned by NXP B.V. and/or its supplier and is protected
# under applicable copyright laws. All rights are reserved. We grant You,
# and any third parties, a license to use this software solely and
# exclusively on NXP products [NXP Microcontrollers such as JN5148, JN5142, JN5139].
# You, and any third parties must reproduce the copyright and warranty notice
# and any other legend of ownership on each copy or partial copy of the
# software.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# Copyright NXP B.V. 2012. All rights reserved
#
#===========================================================================

#ifdef PATCH_ENABLE
#include <patch_abs.inc>

    # Callout table
    CALLOUT_TABLE_ENTRY mib Device_vReset
    CALLOUT_TABLE_ENTRY mib DriverBulb_bOn
    CALLOUT_TABLE_ENTRY mib DriverBulb_bReady
    CALLOUT_TABLE_ENTRY mib DriverBulb_i16Analogue
    CALLOUT_TABLE_ENTRY mib DriverBulb_vInit
    CALLOUT_TABLE_ENTRY mib DriverBulb_vOff
    CALLOUT_TABLE_ENTRY mib DriverBulb_vOn
    CALLOUT_TABLE_ENTRY mib DriverBulb_vSetLevel
    CALLOUT_TABLE_ENTRY mib DriverBulb_vTick
    CALLOUT_TABLE_ENTRY mib eAHI_AttemptCalibration

    # Patch table
    JUMP_TABLE_HEADER .jump_off.appcommon
    JUMP_TABLE_ENTRY MibNwkSecurity_u8ResumeGateway    rom_MibNwkSecurity_u8ResumeGateway
    JUMP_TABLE_ENTRY MibNwkSecurity_u8ResumeStandalone rom_MibNwkSecurity_u8ResumeStandalone
    JUMP_TABLE_ENTRY MibNwkSecurity_bAddSecureAddr     rom_MibNwkSecurity_bAddSecureAddr
    JUMP_TABLE_ENTRY MibNwkSecurity_vSetSecurityKey    rom_MibNwkSecurity_vSetSecurityKey
    JUMP_TABLE_ENTRY MibNwkSecurity_vSetProfile        rom_MibNwkSecurity_vSetProfile
    
#endif
