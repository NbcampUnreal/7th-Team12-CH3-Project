# The Seventh Bullet

> 로그라이크 던전 크롤러 | 3인칭 슈터 | Unreal Engine 5.5

## 개요

**The Seventh Bullet**은 마을에서 의뢰를 수락하고, 던전에서 웨이브 기반 전투를 수행하며, 획득한 소재로 소울 젬을 제작하여 캐릭터를 강화하는 로그라이크 던전 크롤러입니다.

## 게임 플로우

```
메인 메뉴 → 마을(Town) → 의뢰 수락 → 던전(Dungeon) → 웨이브 전투 → 보상 → 마을 복귀
                ↕                         ↕
        스탯 강화 / 제작 / 도박       보스 스테이지(Boss)
```

## 핵심 시스템

### 전투
- **히트스캔 슈팅**: 카메라 기반 레이캐스트, 무기별 스프레드
- **무기 3종**: HandGun / AssaultRifle / ShotGun
- **데미지 파이프라인**: DamageContext → StatusModifier → WeaponModifier → 크리티컬 판정 → 특수 옵션
- **플레이어 스킬**: 쿨타임 기반 액티브 스킬, 보스 패턴 캔슬 가능

### 웨이브 시스템
- **State Machine**: None → Begin → Progress → End → Intermission → Result
- **오브젝트 풀링**: 몬스터 사전 생성 + 비동기 에셋 로딩
- **Day 스케일링**: 진행 일수에 따라 적 스탯 점진적 증가

### 적 AI
- **Behavior Tree + EQS**: 데이터 주도 AI 행동 제어
- **AI Perception**: 시각/청각/예측 기반 타겟 감지
- **보스 시스템**: LevelSequence 연출, 다단계 페이즈, 패턴 컴포넌트(BreakGround/Razor)

### 로그라이크 빌드
- **소울 젬 제작**: 소재 3~4개 조합 → 랜덤 스탯/특수 옵션 부여
- **특수 옵션**: DoubleShot, Lifesteal, Heartsteal, BloodBullet
- **스탯 강화**: 골드로 영구 스탯 업그레이드

### 마을 시설
- 게시판(의뢰) / 제작대(소울 젬) / 상점(스탯 강화) / 창고 / 도박장 / 세이브 포인트
- InteractableInterface 기반 통일된 상호작용

## 기술 스택

| 구분 | 기술 |
|------|------|
| 엔진 | Unreal Engine 5.5 |
| 언어 | C++  |
| AI | Behavior Tree, EQS, AI Perception |
| 데이터 | DataAsset (7종), DataTable (5종) |
| 레벨 | Level Streaming (Persistent + Sub-Level) |
| 연출 | Level Sequence (보스 패턴) |
| 최적화 | Object Pooling, Async Asset Loading |

## 설계 패턴

| 패턴 | 적용 |
|------|------|
| State Machine | 웨이브 상태 전환 |
| Modifier Pipeline | 데미지 계산 |
| Object Pooling | 몬스터/투사체 관리 |
| Component Architecture | 캐릭터, 상호작용 |
| DataAsset | 데이터-로직 분리 |
| Observer (Delegate) | 시스템 간 통신 |
| Subsystem | 전역 매니저 |

## 프로젝트 구조

```
Source/TheSeventhbullet/
├── Character/          # 플레이어 (컴포넌트, 애니메이션, 데미지타입)
├── Enemy/              # 적 (보스, AI, BehaviorTree, 투사체, 노티파이)
├── Wave/               # 웨이브 FSM, 스폰, 레벨 스트리밍 트리거
├── Damage/             # 데미지 컨텍스트, 모디파이어 파이프라인
├── DataAsset/          # 무기/적/아이템/소재/소울젬/보스패턴/UI 데이터
├── Interaction/        # 마을 상호작용 (제작, 강화, 도박, 저장, 재활용)
├── Inventory/          # 인벤토리, 아이템 인스턴스
├── SoulGem/            # 소울 젬 인스턴스
├── UI/                 # 위젯 37개 (HUD, 인벤토리, 제작, 도박, 메뉴)
├── Manager/            # UI/Data/Sound/ProjectilePool 매니저
├── System/             # GameMode, GameState, GameInstance, SubSystem
└── Data/               # 세이브/로드, 데이터 타입 정의
```

## 맵 구성

| 맵                | 용도              |
|------------------|-----------------|
| `L_Town`         | 마을 (상점, 제작, 의뢰) |
| `L_Dungeon`      | 던전 (웨이브 전투)     |
| `L_Boss`         | 보스 스테이지         |
| `L_BossSequence` | 보스 레벨 시퀀스       |


## 빌드

1. Unreal Engine 5.5 설치
2. 프로젝트 폴더에서 `.uproject` 파일 더블클릭
3. Visual Studio에서 빌드 또는 에디터에서 실행
