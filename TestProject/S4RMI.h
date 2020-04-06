#pragma once
#include <Windows.h>
#include <cstdint>

enum class RMI : uint16_t
{
    #define region OfficialRMI

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ AUTH

    //S2C
    Login_EU_Ack = 42003,
    ServerList_Ack = 42005,
    GameData_XBN_Ack = 42008,

    //C2S
    ServerList_Req = 6005,
    Login_EU_Req = 6003,

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ AUTH END
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ CHAT

    //S2C
    Chennel_PlayerList_Ack = 13010,
    Chennel_EnterPlayer_Ack = 13011,
    Message_WhisperChat_Ack = 13014,
    Chat_Login_Ack = 13002,

    //C2S
    Chat_Login_Req = 57002,
    UserData_Three_Ack = 57008,
    Message_WhisperChat_Req = 57010,
    Channel_List_Req = 57025,

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ CHAT END
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ GAME

    //S2C
    Club_Stadium_Edit_MapData_Ack = 53101,
    Login_Reguest_Ack = 53002,
    Player_AccountInfo_Ack = 53024,
    Exp_RefreshInfo_Ack = 53194,
    Item_InventoryInfo_Ack = 53047,
    Item_UseItem_Ack = 53048,
    Room_EnterRoomInfo_Ack = 53007,
    Channel_ListInfo_Ack = 53046,
    Room_PlayerInfoListForEnterPlayer_Ack = 53005,
    Room_LeavePlayerInfo_Ack = 53008,
    Money_RefreshPenInfo_Ack = 53085,
    Money_RefreshCashInfo_Ack = 53083,
    Room_EnterRoomInfo_Ack_2 = 53019,
    Room_ListInfo_Ack_2 = 53020,
    Nuking_User_Ack = 53199,
    Room_Dispose_Ack = 53012,
    Admin_ShowWindow_Ack = 53040,
    Item_UseChangeNick_Ack = 53058,
    Server_Result_Ack = 53198,
    Notice_AdminMessage_Ack = 53126,
    Item_UpdateInventory_Ack = 53049,
    Item_InventroyDelete_Ack = 53062,

    Item_ClearEsperChip_Ack = 53064,

    Room_ChangeRoomInfo_Ack_2 = 53018,
    Room_ChangeRoomInfo_Ack = 53010,

    Room_Mode_Change_Item_Ack = 53023,

    RandomShop_UpdateInfo_Ack = 53090,
    RandomShop_RollingStart_Ack = 53091,
    NewShop_UpdataInfo_Ack = 53081,

    Btc_Clear_Ack = 53171,
    Btc_Sync_Notice = 53172,

    AchieveMission_Ack = 53169,

    Esper_Enchant_Ack = 53071,
    EspherChip_Lv5 = 53072,
    Item_UseEsperChipItem_Ack = 53056,

    CollectBook_UpdateCheck_Ack = 53150,
    CollectBook_InventoryInfo_Ack = 53152,

    EAC_S2C_Data = 53164,

    //C2S
    Item_UseItem_Req = 27010,
    Admin_ShowWindow_Req = 27047,
    Room_Enter_Req = 27006,
    Admin_Action_Req = 27048,
    Room_Leave_Req = 27005,
    RandomShop_RollingStart_Req = 27081,

    Btc_Clear_Req = 27127,

    AchieveMission_Req = 27122,

    Esper_Enchant_PercentUp_Req = 27032,
    Esper_Enchant_Req = 27031,
    Item_UseEsperChip_Req = 27016,

    EAC_C2S_Data = 27108,
    Login_Request_Req = 27002,
    
    Leave_Room_Req = 27005,

    Check_hash_Key_value_req = 27128,
    Character_FirstCreate_Req = 27039,
    Channel_Enter_Req = 27044,
    Channel_Info_Req = 27046,

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ GAME END
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ GAMERULE

    //S2C
    Game_EquipCheck_Ack = 28079,
    Game_KickOutRequest_Ack = 28064,
    Game_KickOutVoteResult_Ack = 28065,
    Game_KickOutState_Ack = 28066,
    Room_LeaveReqeust_Ack = 28004,
    Game_EventMessage_Ack = 28009,
    Room_EnterPlayerForBookNameTags_Ack = 28091,
    Room_EnterPlayerInfoListForNameTag_Ack = 28092,
    Admin_ChangeMP_Ack = 28052,
    Admin_ChangeHP_Ack = 28051,
    Captain_RoundCaptainLifeInfo_Ack = 28067,
    Slaughter_ChangeSlaughter_Ack = 28016,
    Room_LeavePlayer_Ack = 28003,

    Room_ChangeRule_Ack = 28021,
    Room_ChangeRuleNotify_Ack_2 = 28094,
    Room_ChangeRuleNotify_Ack = 28093,

    Game_BriefingInfo_Ack = 28010,
    Arcade_StageBriefing_Ack = 28044,

    Sync_Arena_Battle_Idx = 28098,
    Sync_First_Arena_Battle_Idx = 28099,
    Score_Arena_DrawPlay = 28100,
    Arena_Set_Game_Option_Ack = 28101,
    Arena_Special_Point_Ack = 28102,
    Score_Arena_BattlePlayer_Leave = 28103,
    Arena_LeaderShowdwon = 28104,
    Arena_Draw_HealthPoint_Req = 28105,
    Arena_HarfTimeStatus = 28106,
    Arena_Not_IntrudePlayer = 28107,
    Game_ChangeSubState_Ack = 28012,
    Game_ChangeState_Ack = 28011,
    Game_RefreshGameRuleInfo_Ack = 28041,
    Arcade_SavedateInfo_Ack = 28055,

    //C2S
    Game_EquipCheck_Req = 7049,
    Room_EnterPlayer_Req = 7002,
    Game_KickOutRequest_Req = 7043,
    Game_KickOutVoteResult_Req = 7044,
    Room_IntrudeRound_Req = 7045,
    Game_EventMessage_Req = 7008,
    InGamePlayerResponse_Req = 7054,
    Score_Kill_Req = 7015,
    Score_Offense_Req = 7017,
    Score_Defense_Req = 7019,
    Score_KillAssist_Req = 7016,
    Score_OffenseAssist_Req = 7018,
    Score_DefenseAssist_Req = 7020,
    Score_Suicide_Req = 7024,
    Score_Goal_Req = 7022,
    Room_LeaveReguest_Req = 7003,

    Arena_Special_Point_Req = 7067,
    Arena_Draw_HealthPoint_Ack = 7068,
    Arcade_StageFailed_Req = 7030,
    Arcade_StageClear_Req = 7029,
    Arcade_EnablePlayTime_Req = 7032,
    Arcade_BeginRound_Req = 7028,
    Arcade_StageSelect_Req = 7035,
    Game_AvatarChange_Req = 7012,

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ GAMERULE END
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ RELAY

    //S2C
    Relay_Login_Ack = 56002,
    EnterPlayer_Ack = 56003,

    //C2S
    Relay_Login_Req = 45002,

    //Unk (mostly not Updated)
    RequestAutoPrune = 64514,
    P2PGroup_MemberJoin = 64501,
    P2PGroup_MemberJoin_Unencrypted = 64502,
    NotifyLog = 64008,
    C2S_RequestCreateUdpSocket = 64016,
    C2S_CreateUdpSocketAck = 64017,
    P2P_NotifyDirectP2PDisconnected = 64002,
    NotifyP2PHolepunchSuccess = 64005,
    NotifyJitDirectP2PTriggered = 64012,
    NotifyNatDeviceNameDetected = 64013,

    ReportC2CUdpMessageCount = 64018,
    ReportC2SUdpMessageTrialCount = 64019,

    RequestP2PHolepunch = 64504,

    P2PGroup_MemberJoin_Ack = 64004,

    P2PGroup_MemberLeave = 64506,

    //EVENT

    EVENT_PACKET = 26128,

    Chat = 26120,
    EVENT_P2P_EVENTMESSAGE = 26121,

    //C2C

    StateSync = 7,
    ExposeClubMark = 39,
    SentryGunSpawn = 18,
    FlyStateSync = 21,
    GrenadeSpawn = 22,
    PlayerSpawnReq = 1,
    MindEnergy = 14,
    PlayerRespawnInGame = 45,
    DamageInfo = 4,
    DamageRemoteInfo = 5,

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ RELAY END

    #pragma endregion OfficialRMI
};

enum class S10RMI : uint16_t
{
    #pragma region Season8-10RMI

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ AUTH

    //S2C
    Login_EU_Ack = 5102,

    ServerList_AckREM = 5103,
    ServerList_Ack = 5104,

    //C2S
    ServerList_Req = 5004,
    Login_EU_Req = 5002,

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ AUTH END
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ CHAT

    //S2C
    Chennel_PlayerList_Ack = 16009,
    Chennel_EnterPlayer_Ack = 16010,
    Message_WhisperChat_Ack = 16013,
    Chat_Login_Ack = 16001,

    //C2S
    UserData_Three_Ack = 15007,
    Chat_Login_Req = 15001,
    Message_WhisperChat_Req = 15009,
    Channel_List_Req = 15024,

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ CHAT END
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ GAME

    //S2C
    Club_Stadium_Edit_MapData_Ack = 1169,
    Login_Reguest_Ack = 1001,
    Player_AccountInfo_Ack = 1002,
    Exp_RefreshInfo_Ack = 1104,
    Item_InventoryInfo_Ack = 1005,
    Item_UseItem_Ack = 1011,
    Room_EnterRoomInfo_Ack = 1018,
    Channel_ListInfo_Ack = 1023,
    Room_PlayerInfoListForEnterPlayer_Ack = 1016,
    Room_LeavePlayerInfo_Ack = 1019,
    Money_RefreshPenInfo_Ack = 1053,
    Money_RefreshCashInfo_Ack = 1031,
    Room_EnterRoomInfo_Ack_2 = 1132,
    Room_ListInfo_Ack_2 = 1134,
    Nuking_User_Ack = 1170,
    Room_Dispose_Ack = 1025,
    Admin_ShowWindow_Ack = 1033,
    Item_UseChangeNick_Ack = 1060,
    Server_Result_Ack = 1009,
    EspherChip_Lv5 = 1191,
    Notice_AdminMessage_Ack = 1034,
    Item_UpdateInventory_Ack = 1012,
    Item_InventroyDelete_Ack = 1065,
    NewShop_UpdataInfo_Ack = 1059,

    //C2S
    Login_Request_Req = 2,
    Item_UseItem_Req = 9,
    Admin_ShowWindow_Req = 12,
    Channel_Enter_Req = 14,
    Channel_Info_Req = 16,
    Room_Enter_Req = 17,
    Admin_Action_Req = 22,
    Room_Leave_Req = 10,

    Character_FirstCreate_Req = 48,

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ GAME END
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ GAMERULE

    //S2C
    Game_EquipCheck_Ack = 3078,
    Game_KickOutRequest_Ack = 3063,
    Game_KickOutVoteResult_Ack = 3064,
    Game_KickOutState_Ack = 3065,
    Room_LeaveReqeust_Ack = 3003,
    Game_EventMessage_Ack = 3008,
    Room_EnterPlayerForBookNameTags_Ack = 3090,
    Room_EnterPlayerInfoListForNameTag_Ack = 3091,
    Admin_ChangeMP_Ack = 3051,
    Admin_ChangeHP_Ack = 3050,
    Captain_RoundCaptainLifeInfo_Ack = 3066,
    Slaughter_ChangeSlaughter_Ack = 3015,
    Room_LeavePlayer_Ack = 3002,

    Room_ChangeRule_Ack = 3020,

    //C2S
    Game_EquipCheck_Req = 2049,
    Room_EnterPlayer_Req = 2001,
    Game_KickOutRequest_Req = 2042,
    Game_KickOutVoteResult_Req = 2043,
    Room_IntrudeRound_Req = 2044,
    Game_EventMessage_Req = 2007,
    InGamePlayerResponse_Req = 2053,
    Score_Kill_Req = 2014,
    Score_Offense_Req = 2016,
    Score_Defense_Req = 2018,
    Score_KillAssist_Req = 2015,
    Score_OffenseAssist_Req = 2017,
    Score_DefenseAssist_Req = 2019,
    Score_Suicide_Req = 2023,
    Score_Goal_Req = 2021,
    Room_LeaveReguest_Req = 2002,

    Check_hash_Key_value_req = 123,

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ GAMERULE END
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ RELAY

    //S2C
    Relay_Login_Ack = 11001,
    EnterPlayer_Ack = 11002,

    //C2S
    Relay_Login_Req = 10001,

    //Unk (mostly not Updated)
    RequestAutoPrune = 64514,
    P2PGroup_MemberJoin = 64501,
    P2PGroup_MemberJoin_Ack = 64004,
    P2PGroup_MemberJoin_Unencrypted = 64502,
    P2PGroup_MemberLeave = 64506,
    NotifyLog = 64008,
    C2S_RequestCreateUdpSocket = 64016,
    C2S_CreateUdpSocketAck = 64017,

    S2C_RequestCreateUdpSocket = 64518,
    S2C_CreateUdpSocketAck = 64519,

    P2P_NotifyDirectP2PDisconnected = 64002,
    NotifyP2PHolepunchSuccess = 64005,
    NotifyJitDirectP2PTriggered = 64012,
    NotifyNatDeviceNameDetected = 64013,

    ReportC2CUdpMessageCount = 64018,
    ReportC2SUdpMessageTrialCount = 64019,

    RequestP2PHolepunch = 64504,

    P2PRecycleComplete = 64503,
    NewDirectP2PConnection = 64516,
    NotifyDirectP2PEstablish = 64507,

    NotifyUdpToTcpFallbackByServer = 64511,
    RenewP2PConnectionState = 64515,
    P2P_NotifyDirectP2PDisconnected2 = 64505,

    //EVENT

    EVENT_PACKET = 20025,

    Chat = 20017,
    EVENT_P2P_EVENTMESSAGE = 20018,

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ RELAY END

    #pragma endregion Season8-10RMI
};