#include "common.h"
#include "PacketParser.h"
#include "ChannelSession.h"
#include "PlayerManager.h"
#include "MapInstance.h"


/*
플레이어 피격 시스템

고려 사항
- 플레이어 피격 데미지도 서버에서 계산해서 처리 하는 방식으로 진행
- 몬스터의 레벨과 플레이어 레벨 차이에 따라서 데미지 비율 설정
- 플레이어의 방어력에 따라서 데미지 비율 조정
- 공격 당했을 시 1초 무적 적용

- 반환값 : 플레이어 ID, 플레이어 체력, 받은 데미지

- 서버에서 몬스터 AI를 바탕으로 플레이어를 공격했을 시 피격 여부를 판정하고 피격 시 클라이언트들에게 정보를 내려주기만 하면 된다.

*/

void OnDamagedPacket(PacketContext * ctx)
{
     (void)ctx;
     

}