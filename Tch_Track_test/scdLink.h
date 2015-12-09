
/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2012 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup ALG_LINK_API
    \defgroup ALG_SCD_LINK_API ALG Link: SCD API

    SCD supports two kinds of scene change detect algorithm


    - Tamper Detect (TD) - this is Frame mode of SCD
        - SUPPORTED in ALL platforms
        - Detects camera tamper like hand blocking a camera
        - This is a global or whole frame level algorithm and gives
            YES or NO kind of decision for whole frame for tamper condition.

    - Live motion Detct (LMD) - this is Block mode of SCD
        - SUPPORTED in DM816x, DM814x ONLY
        - Calculates changed pixels in 32x12 block and returns info to user
            on frame to frame basis
        - This info can be used by user to detect motion in specific blocks
            which can be used for a higher motion detect algorithm
        - The data input to this block is live data from capture and
            hence this algorithm is called Live Motion Detect
        - NOTE, only information is returned to user about each block, decision
            of motion or no motion is left to the user

    @{
*/

/**
    \file scdLink.h
    \brief ALG Link: SCD API
*/

#ifndef _SCD_LINK_H_
#define _SCD_LINK_H_

typedef unsigned int UInt32;
typedef unsigned short UInt16;
#ifdef __cplusplus



extern "C" {
#endif

/* Include's    */

/* Define's */

/**
    \brief Max supported SCD channels
*/
#define ALG_LINK_SCD_MAX_CH      (16)


/**
    \brief Max supported frame width for SCD
*/
#define ALG_LINK_SCD_MAX_FRAME_WIDTH               (352)

/**
    \brief Max supported frame height for SCD
*/
#define ALG_LINK_SCD_MAX_FRAME_HEIGHT              (288)

/**
    \brief Max supported block width for SCD in block mode
*/
#define ALG_LINK_SCD_BLK_WIDTH                     (32)

/**
    \brief Max supported block height for SCD in block mode
*/
#define ALG_LINK_SCD_BLK_HEIGHT                    (12)

/**
    \brief Min supported block height for SCD in block mode
*/
#define ALG_LINK_SCD_BLK_HEIGHT_MIN                (10)


/**
    \brief Max number of blocks in horizontal direction
*/
#define ALG_LINK_SCD_MAX_BLK_IN_H_DIR              (ALG_LINK_SCD_MAX_FRAME_WIDTH/ALG_LINK_SCD_BLK_WIDTH)

/**
    \brief Max number of blocks in vertical direction
*/
#define ALG_LINK_SCD_MAX_BLK_IN_V_DIR              (ALG_LINK_SCD_MAX_FRAME_HEIGHT/ALG_LINK_SCD_BLK_HEIGHT)

/**
    \brief Max total number of blocks in a frame
*/
#define ALG_LINK_SCD_MAX_BLOCKS_IN_FRAME          (ALG_LINK_SCD_MAX_BLK_IN_H_DIR * ALG_LINK_SCD_MAX_BLK_IN_V_DIR)

/**
    \ brief Recommended value of thresold that decides time taken before Tamper Alert 
      is issued in the event of Tamper.
*/
#define ALG_LINK_TIME_TO_WAIT_BEFORE_TAMPER_ALERT_THRESOLD  (16)

/**
    \ brief Recommended value of thresold that decides time till tamper state -s re-cket
     after Tamper Event
*/
#define ALG_LINK_TIME_TO_WAIT_AFTER_TAMPER_ALERT_THRESOLD   (8)

/**
 Maximum number of object tracked in any frame processing
*/
#define ALG_LINK_SCD_VA_MAX_OBJECTS    64
/**
 Maximum number of points in a polygon definition
*/
#define ALG_LINK_SCD_VA_MAX_POLY_PTS    16
/**
 Maximum number of ROI in IMD
*/
#define ALG_LINK_SCD_VA_MAX_IMD_ROI    4

/**
 Maximum number of polygon
*/
#define ALG_LINK_SCD_VA_MAX_POLYGONS   ALG_LINK_SCD_VA_MAX_IMD_ROI + 1


/* Constant's */

/**
    \brief SCD sensitivity level
    
    On DM816x, DM814x platform, for Tamper detection  valid range of sensitivity 
    is from 1 - 5; The senistivity value is clipped accordingly to this range.
    For motion detection (LMD), sensitivity range is from 0 - 8.
    
    On DM810x, for Tamper detection  valid range of sensitivity 
    is from 0 - 8;
*/
typedef enum
{
    ALG_LINK_SCD_SENSITIVITY_MIN          =  0,
    /**< Minimum sensitivity    */

    ALG_LINK_SCD_SENSITIVITY_VERYLOW      =  1,
    /**< Very low sensitivity    */

    ALG_LINK_SCD_SENSITIVITY_LOW          =  2,
    /**< Low sensitivity    */

    ALG_LINK_SCD_SENSITIVITY_MIDLO        =  3,
    /**< Mid-Lo sensitivity    */

    ALG_LINK_SCD_SENSITIVITY_MID          =  4,
    /**< Medium sensitivity    */

    ALG_LINK_SCD_SENSITIVITY_MIDHI        =  5,
    /**< Mid-Hi sensitivity    */

    ALG_LINK_SCD_SENSITIVITY_HIGH         =  6,
    /**< High sensitivity    */

    ALG_LINK_SCD_SENSITIVITY_VERYHIGH     =  7,
    /**< Very High sensitivity    */

    ALG_LINK_SCD_SENSITIVITY_MAX          =  8
    /**< Maximum sensitivity */

} AlgLink_ScdSensitivity;

/**
    \brief SCD mode
*/
typedef enum
{
    ALG_LINK_SCD_DETECTMODE_DISABLE              = 0,
    /**< Disable SCD */

    ALG_LINK_SCD_DETECTMODE_MONITOR_FULL_FRAME   = 1,
    /**< Frame based SCD = Tamper Detect (TD) mode

        SUPPORTED in ALL platforms
    */
    ALG_LINK_SCD_DETECTMODE_MONITOR_BLOCKS       = 2,
    /**< Block based SCD - Live motion detect (LMD) mode

        ONLY SUPPORTED in DM816x, DM814x
    */

    ALG_LINK_SCD_DETECTMODE_MONITOR_BLOCKS_AND_FRAME = 3,
    /**< Both frame and block based SCD - TD+LMD mode

        ONLY SUPPORTED in DM816x, DM814x
    */

    ALG_LINK_SCD_DETECTMODE_IMD      = 4,
    /**< Frame based Inteligent motion detect (IMD) mode

        ONLY SUPPORTED in DM816x, DM814x
    */

    ALG_LINK_SCD_DETECTMODE_SMETA    = 8,
    /**< Streaming metaData From Algorithm Engine

        ONLY SUPPORTED in DM816x, DM814x
    */

    ALG_LINK_SCD_DETECTMODE_TRIPZONE = 16,
    /**< Frame based Trip Zone mode

        ONLY SUPPORTED in DM816x, DM814x
    */

    ALG_LINK_SCD_DETECTMODE_COUNTER  = 32
    /**< Frame based Object Count mode
         Event detected when objects move across top and bottom halves of the image or 
         event detected when objects move across left and right halves of the image.
        ONLY SUPPORTED in DM816x, DM814x
    */

} AlgLink_ScdMode;

/**
    \brief  SCDVadirection
            Defines various enumerated types 
            for different types of object movements
*/
typedef enum
{
  ALG_LINK_SCD_VA_DIRECTION_UNSPECIFIED  = 0,
  ALG_LINK_SCD_VA_DIRECTION_ZONEA2B      = 1,
  ALG_LINK_SCD_VA_DIRECTION_ZONEB2A      = 2
} SCDVaDirection;

/**
    \brief SCDVaRoiType
            Defines various enumerated types 
            for different kind of ROI.
 */
typedef enum
{
  ALG_LINK_SCD_VA_NO_ROI   = 0,
  ALG_LINK_SCD_VA_IMD      = 1,
  ALG_LINK_SCD_VA_TZ_1A    = 2,
  ALG_LINK_SCD_VA_TZ_1B    = 4,
  ALG_LINK_SCD_VA_OC_1A    = 8, /* Depricated, Not required for Object Count Algo */
  ALG_LINK_SCD_VA_OC_1B    = 16,/* Depricated, Not required for Object Count Algo */
  ALG_LINK_SCD_VA_META     = 32
} SCDVaRoiType;

/**
    \brief SCD Status for Tamper Detect
*/
typedef enum
{
    ALG_LINK_SCD_DETECTOR_UNAVAILABLE =-1,
    /**< Status NOT available   */

    ALG_LINK_SCD_DETECTOR_NO_CHANGE   = 0,
    /**< No scene change detected - Tamper NOT detected */

    ALG_LINK_SCD_DETECTOR_QUALIFYING  = 1,
    /**< NOT USED right now */

    ALG_LINK_SCD_DETECTOR_CHANGE      = 2
    /**< Scene change detected - Tamper detected */

} AlgLink_ScdOutput;

/**
 *  @enum       AlgLink_ScdVaDetectorResult
 *  @brief      Defines various enumerated types 
 *              for events detected by SCD Video Analytics Engine
 */
typedef enum
{
  ALG_LINK_SCD_DETECTOR_NONE      = 0,
  ALG_LINK_SCD_DETECTOR_TAMPER    = 32,
  ALG_LINK_SCD_DETECTOR_IMD       = 64,
  ALG_LINK_SCD_DETECTOR_TRIPZONE  = 128,
  ALG_LINK_SCD_DETECTOR_COUNTER   = 256,
  ALG_LINK_SCD_DETECTOR_SMETA     = 512,
  ALG_LINK_SCD_DISABLED           = 1024,
  ALG_LINK_SCD_INITIALIZING       = 2048
} AlgLink_ScdVaDetectorResult;

/* Control Command's    */

/**
    \ingroup ALG_LINK_API_CMD
    \addtogroup ALG_SCD_LINK_API_CMD  ALG Link: SCD API Control Commands

    @{
*/

/**
    \brief Link CMD: Set ignore lights ON flag for a channel

    SUPPORTED in ALL platforms

    AlgLink_ScdChParams.chId specifies the channel number
    AlgLink_ScdChParams.frmIgnoreLightsON specifies the if lights ON are ignored or NOT

    Other fields are not used

    \param AlgLink_ScdChParams *   [IN]  parameters
*/
#define ALG_LINK_SCD_CMD_SET_CHANNEL_IGNORELIGHTSON    (0x7001)

/**
    \brief Link CMD: Set ignore lights OFF flag for a channel

    SUPPORTED in ALL platforms

    AlgLink_ScdChParams.chId specifies the channel number
    AlgLink_ScdChParams.frmIgnoreLightsOFF specifies the if lights OFF are ignored or NOT

    Other fields are not used

    \param AlgLink_ScdChParams *   [IN]  parameters
*/
#define ALG_LINK_SCD_CMD_SET_CHANNEL_IGNORELIGHTSOFF   (0x7002)

/**
    \brief Link CMD: Set SCD detect mode for a channel

    ONLY SUPPORTED in DM816x, DM814x

    AlgLink_ScdChParams.chId specifies the channel number
    AlgLink_ScdChParams.mode specifies the new SCD mode

    Other fields are not used

    \param AlgLink_ScdChParams *   [IN]  parameters
*/
#define ALG_LINK_SCD_CMD_SET_CHANNEL_MODE              (0x7003)

/**
    \brief Link CMD: Set SCD sensitivity for a channel

    SUPPORTED in ALL platforms

    AlgLink_ScdChParams.chId specifies the channel number
    AlgLink_ScdChParams.frmSensitivity specifies the new SCD sensitivity

    On DM816x, DM814x platform, for Tamper detection  valid range of sensitivity 
    is from 1 - 5; The senistivity value is clipped accordingly to this range.
    For motion detection (LMD), sensitivity range is from 0 - 8.
    
    On DM810x, for Tamper detection  valid range of sensitivity 
    is from 0 - 8;

    Other fields are not used

    \param AlgLink_ScdChParams *   [IN]  parameters
*/
#define ALG_LINK_SCD_CMD_SET_CHANNEL_SENSITIVITY       (0x7004)

/**
    \brief Print SCD statistics

    In DM810x,
    - this also prints OSD statistics

    In DM816x, DM814x, OSD statistics print not implemented as of now

    \param NONE
*/
#define ALG_LINK_SCD_CMD_PRINT_STATISTICS              (0x7005)

/**
    \brief Set block configuration for a channel

    ONLY SUPPORTED in DM816x, DM814x

    \param AlgLink_ScdChblkUpdate   *   [IN]   Block update parameters
*/
#define ALG_LINK_SCD_CMD_SET_CHANNEL_BLOCKCONFIG       (0x7006)

/**
    \brief Get SCD status at frame level for all channels

    SUPPORTED in ALL platforms

    \param AlgLink_ScdAllChFrameStatus   *   [OUT]   status
*/
#define ALG_LINK_SCD_CMD_GET_ALL_CHANNEL_FRAME_STATUS   (0x7007)


/**
    \brief Reset SCD algorithm state for a given channel

    SUPPORTED in ALL platforms

    \param AlgLink_ScdChCtrl   *   [IN]   parameters
*/
#define ALG_LINK_SCD_CMD_CHANNEL_RESET                 (0x7008)

/**
    \brief Set Analysics configuration for a channel

    ONLY SUPPORTED in DM816x, DM814x

    \param AlgLink_ScdChVaParams   *   [IN]   Update Va parameters
*/
#define ALG_LINK_SCD_CMD_SET_CHANNEL_VA_PARAMS       (0x7009)

/**
    \brief Print Video Analysics configuration of a channel

    ONLY SUPPORTED in DM816x, DM814x

*/
#define ALG_LINK_SCD_CMD_PRINT_ALLCHANNEL_VA_PARAMS       (0x700A)

/* @} */

/* Data structure's */

/**
    \brief Block configuration - part of AlgLink_ScdChParams
*/
typedef struct
{
    UInt32 sensitivity;
    /**< Blocks's sensitivity setting for change detection
            For valid values see \ref AlgLink_ScdSensitivity
         */

    UInt32 monitored;
    /**< TRUE: monitor block for change detection
        FALSE: ignore this block
    */
} AlgLink_ScdblkChngConfig;

/**
  \brief AlgLink_ScdVaGuid
  This structure contains all the parameters which uniquely
  identify a a detected object. Basically it is signature 
  of detected object.
*/
typedef struct
{
  UInt32  data1; 
  /**< Micro second part of the time stamp
       of the image in which this obejct was 
       detected. 
   */

  UInt32 data2;
  /**< Object id of the detected object
   */

  UInt32 data3;
  /**< tz_minuteswest part of time stamp of the 
       frame in which this object was detected.
    */

  UInt32 data4;
  /**< Second part of the time stamp
       of the image in which this obejct was 
       detected. 
   */

} AlgLink_ScdVaGuid;

/**
  \brief AlgLink_ScdVaBox
  This structure various parameter to define a rectangular box.
*/
typedef struct
{
    UInt32  startX;
    UInt32  startY;
    UInt32  width;
    /**< Width of the box
        */
    UInt16  height;
    /**< Height of the box
     */
    UInt32  boxArea;
    /**< Area of the rectangular box containing 
           the object.
                       */
    UInt32  objArea;
    /**< Area of actual object.
    */
} AlgLink_ScdVaBox;

/**
  \brief AlgLink_ScdVaPoint

  This structure defines a point interms of X & Y cordinate.
*/
typedef struct
{
    UInt32 x;
    UInt32 y;
} AlgLink_ScdVaPoint;

/**
  \brief AlgLink_ScdVaVector2D
                                  
  This structure defines a vector interms of X & Y cordinate.
*/
typedef struct
{
    UInt32 x;
    UInt32 y;
} AlgLink_ScdVaVector2D;

/**
  
  \brief AlgLink_ScdVaObject
  This structure defines a Object interms of Width & Height cordinate.
*/
typedef struct
{
    UInt32 width;
    /**< Width of Object 
     */

    UInt32 height;
    /**< Height of Object 
     */
} AlgLink_ScdVaObject;

/**
    \brief AlgLink_ScdVaZoneCfg
    Polygon/Zone configuration - part of AlgLink_ScdChVaParams
*/
typedef struct
{
    UInt32 zoneId; 
    /**< Zone ID
     */

    UInt32 activeZone; 
    /**< Zone Enabled/Disabled - 1: Zone enabled,
                       0: Zone disabled */

    UInt32 zoneType; 
    /**< Zone Type; For valid values see \ref SCDVaRoiType */

    UInt32 zoneDirection; 
    /**< Zone Direction; For valid values see \ref SCDVaDirection */

    UInt32 noOfPoints;
    /**< Number of points in the current polygon/zone.
         Max 16 points are supported by Algorithm
         */

    AlgLink_ScdVaPoint pixLocation[ALG_LINK_SCD_VA_MAX_POLY_PTS];
    /**< X,Y - Location of a pixel in a frame.
    */
} AlgLink_ScdVaZoneCfg;


/**
    \brief SCD Va parameters that can be changed dynamically.
     Part of AlgLink_ScdChParams.
*/
typedef struct
{
    UInt32 chId;
    /**< Channel number, 0.. \ref ALG_LINK_SCD_MAX_CH-1 */

    UInt32 mode;
    /**< Va Detection mode, for valid values see \ref AlgLink_ScdMode */

    UInt32 sensitiveness;
    /**< DMVL sensitivoty. Valid for Tamper, TZ, OC, IMD and SMETA
    */

    UInt32  verboseLevel;
    /**< Enable Debug Print */

    UInt32 objectInView; 
    /**< People/Vehicle present flag - 
            3: Both Vehicle/Person in view
            2: Person in View, 
            1: Vehicle in View 
            0: Nothing is view; Note: This is invalid, Atleast 1 should be in view for Any algorithm 

            Valid only for TZ, OC, SMETA
    */

    UInt32 direction; 
    /**< Va direction , for valid values see \ref SCDVaDirection
         Valid only for TZ, OC.
            */
    UInt32 orientation; 
    /**< Valid only for OC. Valid values. 0 or 1. 
            0 -> Monitor vertical movement.
            1 -> Monitor horozontal movement. 
      */
    UInt32 detectionRate;
   /**< Controls how frequently object detection are done by DMVAL. 
         It defines how many times in a second the object
         detection should happen.
         Valid Only for TZ and OC.
      */
    UInt32 tamperResetCnt;
    /**< This parameter is accepted in terms of seconds and it 
       is used to control the duration after which camera 
       tamper state is reseted and stops reporting the 
       tamper event even is camera is blocked/moved/shaking/
       defocused etc.. Recomended and default value for this 
       parameter is 20 seconds. However any positive number 
       is accepted from interface.
     */

    UInt32 tamperBGRefreshInterval;
    /**< Set to 1 to n for the maximum number of seconds to wait for pre-tamper
         conditions to return following a tamper event */

    AlgLink_ScdVaObject minPersonSize;
    /**< Person min width, Height. valid only for OC, TZ, SMETA, IMD
    */

    AlgLink_ScdVaObject minVehicleSize;
    /**< Vehicle min width Height. Valid only for TZ, SMETA, IMD
    */

    AlgLink_ScdVaObject maxPersonSize;
    /**< Person max width Height. Valid Only TZ,SMETA,IMD.
    */

    AlgLink_ScdVaObject maxVehicleSize;
    /**< Vehicle max width Height. Valid only for TZ, SMETA, IMD.
    */

    UInt32 numZone; 
    /**< Num of Zones - 0 to ALG_LINK_SCD_VA_MAX_IMD_ROI - 1 */
    
    AlgLink_ScdVaZoneCfg zoneCfg[ALG_LINK_SCD_VA_MAX_IMD_ROI];
    /**< Polygon/Zone configuration
     */
} AlgLink_ScdChVaParams;

/**
    \brief SCD channel parameters that can be changed dynamically
*/
typedef struct
{
    UInt32 chId;
    /**< Channel number, 0.. \ref ALG_LINK_SCD_MAX_CH-1 */

    UInt32 mode;
    /**< SCD mode, for valid values see \ref AlgLink_ScdMode */

    UInt32 frmIgnoreLightsON;
    /**< TRUE: ignore sudden, significantly brighter avg. scene luminance, FALSE: Do no ignore */

    UInt32 frmIgnoreLightsOFF;
    /**< TRUE: ignore sudden, significantly darker avg. scene luminance, FALSE: Do not ignore */

    UInt32 frmSensitivity;
    /**< Internal threshold for scd change tolerance; globally applied to entire frame
            For valid values see \ref AlgLink_ScdSensitivity
    */

    UInt32 frmEdgeThreshold;
    /**< Set minimum number of edge pixels required to indicate non-tamper event

        Set to 100 for now ALWAYS
    */

    UInt32 inputFrameRate;
    /**< Frames per second fed to the SCD. 
         If set at 0 value, value of link level frame rate is applied.
     */

    UInt32 outputFrameRate;
    /**< Frames per second alogorithm is operated at 
         If set at 0 value, value of link level frame rate is applied.
    */

    UInt32 blkNumBlksInFrame;
    /**< Number of blocks in a frame for block based SCD

        NOT SUPPORTED in DM810x

        ONLY valid when
            AlgLink_ScdChParams.mode =
                ALG_LINK_SCD_DETECTMODE_MONITOR_BLOCKS
                    OR
                ALG_LINK_SCD_DETECTMODE_MONITOR_BLOCKS_AND_FRAMES
    */

    AlgLink_ScdblkChngConfig blkConfig[ALG_LINK_SCD_MAX_BLOCKS_IN_FRAME];
    /**< Block level configuration */

    UInt32 useDefaultVaAlgCfg;
    /**< TRUE : Use default VA configuraion.
         FALSE: Use VA config provided by user at create time call.
     */
    AlgLink_ScdChVaParams  scdVaAlgCfg;
    /**< SCD Video Analytics Configuration 
    */

} AlgLink_ScdChParams;

/**
    \brief Block level config - part of AlgLink_ScdChblkUpdate
*/
typedef struct
{
   UInt32               blockId;
   /**< Block ID for which parameters need to be changed
        Valid values, 0 .. ALG_LINK_SCD_MAX_BLOCKS_IN_FRAME-1 */

   UInt32               sensitivity;
   /**< Blocks's sensitivity setting for change detection
            For valid values see \ref AlgLink_ScdSensitivity
     */


   UInt32               monitorBlock;
   /**< TRUE: monitor block for change detection
        FALSE: ignore this block
   */

} AlgLink_ScdChBlkConfig;

/**
    \brief Block's whose config needs to be updated
*/
typedef struct
{
    UInt32                 chId;
    /**< Channel number, 0..ALG_LINK_SCD_MAX_CH-1 */

    UInt32                 numValidBlock;
    /**< Number of blocks to be updated,
        i.e number of valid entries in AlgLink_ScdChblkUpdate.blkConfig[] arrary
    */

    AlgLink_ScdChBlkConfig blkConfig[ALG_LINK_SCD_MAX_BLOCKS_IN_FRAME];
    /**< Config of each block that is to be updated */

}AlgLink_ScdChblkUpdate;

/**
    \brief SCD status of a block - part of AlgLink_ScdChStatus
*/
typedef struct
{
    UInt32 numFrmsBlkChanged;
    /**< Number of consecutive frames with motion in block */

    UInt32 numPixelsChanged;
    /**< Raw number of block pixels that were changed */

} AlgLink_ScdblkChngMeta;

/**
    \brief SCD status that is returned with event VSYS_EVENT_TAMPER_DETECT

    When tamper is detect a event, VSYS_EVENT_TAMPER_DETECT, is to Linux.
    Along with the event a pointer to status is sent across

    ONLY valid when ALG_LINK_SCD_ENABLE_EVENT_NOTIFY is 1

    NOT SUPPORTED for DM810x
*/
typedef struct
{
    UInt32 chId;
    /**< SCD channel number on which tamper event was detected

        Valid values, 0..ALG_LINK_SCD_MAX_CH-1
    */

    UInt32 frmResult;
    /**< SCD change detection result from entire frame
            For valid values, see \ref AlgLink_ScdOutput
    */

} AlgLink_ScdChStatus;

/**
    \brief AlgLink_ScdVaObjMeta
    This structure defines various information for detected object.
 */
typedef struct 
{
  UInt32                area;
  /**< Area of the detected object  */

  UInt32                xsum; 
  /**< Sum of horizontal coordinates of object blob pixels. */

  UInt32                ysum;
  /**< Sum of vertical coordinates of object blob pixels */

  UInt32                zoneId;
  /**< Identification number for the zone in which the object occurs */

  UInt32                result;
  /**< Indicates the event triggered by this object */

  AlgLink_ScdVaGuid     objID;
  /**< Unique identification number for this object  */

  AlgLink_ScdVaBox      objBox;
  /**< Bounding box corresponding to object location in the image. */

  AlgLink_ScdVaPoint    objCentroid;
  /**< Center (1st order moment) of the object blob center. */

  AlgLink_ScdVaVector2D objVelocity;
  /**< Current measured velocity of the object?s centroid  */

  UInt32            dir;
  /**< Direction of object motion. */

} AlgLink_ScdVaObjMeta;

/**
    \brief SCD status of block level SCD

    This results is generated for every frame and is available in
    the output queue (\ref ALG_LINK_SCD_OUT_QUE) of the SCD Link.

    This status is also available with the event VSYS_EVENT_MOTION_DETECT
*/
typedef struct
{
    UInt32 chId;
    /**< Channel number, 0 .. \ref ALG_LINK_SCD_MAX_CH-1 */

    UInt32 mode;
    /**< Va Detection mode, for valid values see \ref AlgLink_ScdMode */

    UInt32 frmResult;
    /**< SCD change detection result from entire frame */

    UInt32 objectCount;
    /**< Number of objects detected till now. 
         Populated only when Object count algorithm is enabled.
     */

    UInt32 numObjects;
    /**< Number of objects detected in current frame. 
         Populated only when a valid event is detected
     */

    AlgLink_ScdblkChngConfig blkConfig[ALG_LINK_SCD_MAX_BLOCKS_IN_FRAME];
    /**< Linear array of configuration of frame blocks that
         scd will monitor for motion detection (configuration)
    */

    AlgLink_ScdblkChngMeta  blkResult[ALG_LINK_SCD_MAX_BLOCKS_IN_FRAME];
    /**< SCD change detection result from individual frame tiles/blocks
         array length = \ref ALG_LINK_SCD_MAX_BLOCKS_IN_FRAME
    */
    int        polygonRtUpdated;
    /**< Polygon Configuration Updated 
    */

    AlgLink_ScdVaZoneCfg zoneCfg[ALG_LINK_SCD_VA_MAX_IMD_ROI];
    /**< It contains various information about zones defind for current channel.
     */
    UInt32 numPolygons;
    /**< Number of Polygon/Zone defined.
     */

    AlgLink_ScdVaObjMeta objMetadata[ALG_LINK_SCD_VA_MAX_OBJECTS];
    /**< It contains various information about detected object.
     */

} AlgLink_ScdResult;


/**
    \brief SCD frame level status information for all CHs
*/
typedef struct
{
    UInt32 numCh;
    /**< Channel number, 0 .. \ref ALG_LINK_SCD_MAX_CH-1 */

    AlgLink_ScdChStatus chanFrameResult[ALG_LINK_SCD_MAX_CH];
    /**< SCD frame level state. For valid values see, AlgLink_ScdOutput */
} AlgLink_ScdAllChFrameStatus;

/**
    \brief Data structure to select CH on which reset operation is done
*/
typedef struct
{
    UInt32 chId;
    /**< Channel number, 0.. \ref ALG_LINK_SCD_MAX_CH-1 */

} AlgLink_ScdChCtrl;

/**
    \brief SCD link create parameters
*/
typedef struct
{
    UInt32 maxWidth;
    /**< Set the maximum width (in pixels) of video frame that scd will process */

    UInt32 maxHeight;
    /**< Set the maximum height (in pixels) of video frame that scd will process */

    UInt32 maxStride;
    /**< Set the maximum video frame pitch/stride of the images in the video buffer*/

    UInt32 numSecs2WaitB4Init;
    /**< Set the number of seconds to wait before initializing SCD monitoring.
         This wait time occurs when the algorithm instance first starts and
         allows video input to stabilize.*/

    UInt32 numSecs2WaitB4FrmAlert;
    /**< Set the number of seconds to wait before signaling a frame-level scene change event.*/

    UInt32 numSecs2WaitAfterFrmAlert;
    /**< Set to 1 to n for the maximum number of seconds to wait for pre-tamper
         conditions to return following a tamper event */

    UInt32 thresold2WaitB4FrmAlert;
    /**< Set the thresold to wait before signaling a frame-level scene change event.
         It can take values from 0-32. Recommended value for this 
         ALG_LINK_TIME_TO_WAIT_BEFORE_TAMPER_ALERT_THRESOLD.  User can tune this
         Value to change response time. */

    UInt32 thresold2WaitAfterFrmAlert;
    /**< Set thresold to wait for pre-tamper conditions to return following a 
         tamper event. It can take values from 0-32.  Recommended value for this 
         ALG_LINK_TIME_TO_WAIT_AFTER_TAMPER_ALERT_THRESOLD.  User can tune this
         Value as per requirement. */

    UInt32 inputFrameRate;
    /**< Frames per second fed to the SCD 
         This param is used for LMD. If I/p-O/p FPS of respective channel is not
         provided in chDefaultParams then this values is used as default value.
      */

    UInt32 outputFrameRate;
    /**< Frames per second alogorithm is operated at 
         This param is used for LMD. If O/p-O/p FPS of respective channel is not 
         provided in chDefaultParams then this values is used as default value.
    */

    AlgLink_ScdChParams chDefaultParams[ALG_LINK_SCD_MAX_CH];
    /**< Pointer to array of channel params used to configure ScdAlgLink.

        - SCD need not be enabled for all channels.
        - The number of channels are set via AlgLink_ScdCreateParams.numValidChForSCD
        - The valid channels or channels selected for SCD as specified via
            AlgLink_ScdCreateParams.chDefaultParams[x].chId
    */

    UInt32 numBufPerCh;
    /**< Number of buffer to allocate per channel - ONLY applicable for LMD mode of operation */

    UInt32 enableMotionNotify;
    /**< Enable CPU side Live Motion detect Notify to A8 by VSYS_EVENT_MOTION_DETECT command.
      * If disabled Application has to take care motion detection part from
      * block metaData that is exported by DSP to A8 */

    UInt32 enableTamperNotify;
    /**< Enable CPU side Tamper detect Notify to A8 by VSYS_EVENT_TAMPER_DETECT command
      * If disabled Application has to take care Tamper detection notify part
      * at A8 by calling ALG_LINK_SCD_CMD_GET_ALL_CHANNEL_FRAME_STATUS command
      * with appropriate structure */

    UInt32 enableVaEvntNotify;
    /**< Enable CPU side VA event display.
     */

    UInt32 numValidChForSCD;
    /**< Set the maximum channels on which scd will run.

            This determines the number of channels info that is valid in

            AlgLink_ScdCreateParams.chDefaultParams[]

         */
    UInt32 maxNumVaChan;
    /**< Set the maximum channels on which DMVAL algo (TZ, OC, IMD etc. will run.
         */
} AlgLink_ScdCreateParams;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

