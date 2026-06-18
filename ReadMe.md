# StoryFlow

`StoryFlow`는 Unreal Engine 5 프로젝트에서 **장소/상황 단위 Scene**과 그 안의 **진행 단계 Shot**을 그래프로 편집하고, 런타임에서 Shot 실행, Branch 분기, Scene 전이, 레벨 로딩을 처리하는 스토리 플로우 플러그인이다.

이 문서는 `Plugins/StoryFlow`의 **현재 코드 기준**으로 유지되는 기술 README다. 실제 사용 절차 중심 문서는 [UserGuide.md](./UserGuide.md)를 참고한다.

---

## 핵심 요약

- `StorySceneAsset` 하나가 하나의 Scene 흐름을 가진다.
- Scene은 `SceneID`, `TargetLevel`, `SceneTemplate`, Shot/Branch 그래프를 가진다.
- Entry는 시작 Shot을 결정한다.
- Shot은 실제 진행 단계이며 단일 `Next` 링크를 가진다.
- Branch는 여러 출력 핀 중 다음 경로를 선택한다.
- Transition은 다음 `SceneID`로 전이하는 에디터 노드이며 런타임 객체를 만들지 않는다.
- 저장/복원 최소 단위는 `FStoryFlowRef = SceneID + ShotID`다.
- `Debug StoryFlow` 콘솔 명령으로 런타임 디버그 오버레이를 토글할 수 있다.

---

## 모듈 구성

플러그인은 `CommonLibrary` 플러그인에 의존한다.

| 모듈 | 역할 |
| --- | --- |
| `StoryFlow` | 런타임 타입, Subsystem, Scene/Shot/Branch 실행, 로딩, 디버그 콘솔 명령 |
| `StoryFlowEditor` | StoryScene 에셋 에디터, 그래프 노드/스키마, Compile/검증, Blueprint 팩토리, 에디터 UI |

---

## 주요 런타임 타입

### ID / 진행 상태

파일: `Source/StoryFlow/Public/StoryFlowDefines.h`

| 타입 | 의미 |
| --- | --- |
| `FStorySceneID` | Scene 식별자. 내부 값은 `FName` |
| `FStoryShotID` | Shot 식별자. 내부 값은 `FName` |
| `FStoryBranchID` | Branch 식별자. 내부 값은 `FName` |
| `FStoryFlowRef` | 저장/복원용 진행 참조. `SceneID`, `ShotID` 포함 |

디자이너 노출 정책:

- `SceneID`는 SceneAsset에서 편집 대상이다.
- `ShotID`는 Shot 노드 데이터에서 표시만 되고 디자이너가 직접 수정하지 않는다.
- `BranchID`는 Details 패널에서 숨긴다.
- 그래프 노드 제목에는 내부 ID를 과도하게 노출하지 않는다.

### `UStorySceneAsset`

파일: `Source/StoryFlow/Public/StorySceneAsset.h`

Scene 전체를 나타내는 `UPrimaryDataAsset`이다.

주요 데이터:

- `_SceneID`
- `_DisplayName`
- `_Description` — Entry 노드 comment bubble로 표시되는 Scene 설명
- `_TargetLevel`
- `_EntryLink` — Compile 결과로 채워지는 Entry의 내부 시작 링크, Details에는 숨김
- `_SceneTemplate`
- `_ShotNodes`
- `_BranchNodes`
- `_EditorGraph` (`WITH_EDITORONLY_DATA`)

주요 조회 함수:

- `FindShotNode(const FStoryShotID&)`
- `FindBranchNode(const FStoryBranchID&)`

### `UStorySceneNodeData`

파일: `Source/StoryFlow/Public/StorySceneNodeData.h`

Shot 노드에 대응하는 데이터 오브젝트다.

주요 데이터:

- `_ShotID` — 자동 발급, Details에서는 읽기 전용
- `_DisplayName` — Shot 노드 제목의 두 번째 줄로 표시. 비어 있으면 `ShotTemplate` 설정 시 템플릿 이름으로 자동 채움
- `_Description` — 그래프 comment bubble로 표시
- `_ShotTemplate` — 실행할 `UStoryShotBase` 인스턴스 템플릿
- `_NextLink` — Compile 결과로 채워지는 내부 링크, Details에는 숨김

### `UStoryBranchNodeData`

파일: `Source/StoryFlow/Public/StoryBranchNodeData.h`

Branch 노드에 대응하는 데이터 오브젝트다.

주요 데이터:

- `_BranchID` — 자동 발급, Details에는 숨김
- `_DisplayName` — Branch 노드 제목의 두 번째 줄로 표시. 비어 있으면 `BranchTemplate` 설정 시 템플릿 이름으로 자동 채움
- `_Description` — 그래프 comment bubble로 표시
- `_BranchTemplate` — 실행할 `UStoryBranchBase` 인스턴스 템플릿. 출력 핀 개수와 표시명도 이 템플릿의 Outputs가 정의한다
- `_NextLinksByPinIndex` — Compile 결과로 채워지는 내부 링크 맵, Details에는 숨김

### `FStorySceneBranchLink`

파일: `Source/StoryFlow/Public/StorySceneNodeData.h`

다음 목적지 하나를 표현한다.

- `NextShotID`
- `NextBranchID`
- `NextSceneID`

현재 설계상 한 링크에는 셋 중 하나만 유효한 상태를 기대한다.

### 실행 베이스 클래스

| 클래스 | 역할 | Blueprint 오버라이드 |
| --- | --- | --- |
| `UStorySceneBase` | Scene 시작/종료 로직 | `OnEnterScene`, `OnExitScene` |
| `UStoryShotBase` | Shot 시작/Tick/종료/완료 | `OnEnterShot`, `OnTickShot`, `OnExitShot`, `FinishShot` |
| `UStoryBranchBase` | Branch 출력 정의/분기 선택 | `BranchOutputs`, `SelectNextIndex(int32 NextCount)` |

각 실행 객체는 템플릿을 그대로 실행하지 않고, 런타임에서 `DuplicateObject`로 인스턴스를 만들어 사용한다.

### `UStorySceneRegistryAsset`

파일: `Source/StoryFlow/Public/StorySceneRegistryAsset.h`

`SceneID -> StorySceneAsset` resolve에 사용하는 레지스트리 에셋이다.

- 다음 Scene 전이
- `StartFromScene`
- Compile 검증

에서 사용한다.

### `UStoryFlowDeveloperSettings`

파일: `Source/StoryFlow/Public/StoryFlowDeveloperSettings.h`

Project Settings에 저장되는 전역 설정이다.

| 설정 | 의미 |
| --- | --- |
| `_StorySceneRegistry` | SceneID 조회용 레지스트리 |
| `_LoadingLevel` | TargetLevel 전환 전에 진입할 로딩 레벨 |
| `_MinimumLoadingLevelDuration` | 로딩 레벨 최소 체류 시간 |
| `_MinimumLoadingLevelProgressCurve` | 표시용 로딩 진행률 보정 커브 |

---

## 런타임 흐름

### 시작 API

파일: `Source/StoryFlow/Public/StoryFlowSubsystem.h`

```cpp
UStoryFlowSubsystem* StoryFlow = GetGameInstance()->GetSubsystem<UStoryFlowSubsystem>();
StoryFlow->StartFromScene(FStorySceneID(TEXT("Scene_Intro")));
```

또는 저장된 진행 상태에서 복원한다.

```cpp
FStoryFlowRef Ref;
Ref.SceneID = FStorySceneID(TEXT("Scene_Intro"));
Ref.ShotID = FStoryShotID(TEXT("Shot_003"));
StoryFlow->StartFromRef(Ref);
```

게임 시작 시 자동으로 StoryFlow를 시작하는 프로젝트 코드는 `GameMode::StartPlay()`에서 `Super::StartPlay()` 이후에 호출하는 것을 권장한다. 이렇게 하면 월드 Actor/PlayerController의 BeginPlay가 먼저 dispatch된 뒤 첫 Shot의 `EnterShot`이 실행된다.

```cpp
void AMyGameMode::StartPlay()
{
	Super::StartPlay();

#if WITH_EDITOR
	if (UStoryFlowSubsystem::IsEditorPlayFromShotSession())
	{
		return;
	}
#endif

	UStoryFlowSubsystem* StoryFlow = GetGameInstance()->GetSubsystem<UStoryFlowSubsystem>();
	if (StoryFlow)
	{
		StoryFlow->StartFromScene(FStorySceneID(TEXT("Scene_Intro")));
	}
}
```

Story Scene Editor의 Shot 노드 Play는 PIE 시작 전에 `UStoryFlowSubsystem::IsEditorPlayFromShotSession()`을 true로 표시한다. 프로젝트에서 일반 PIE 시작 시 Intro Scene을 자동 실행한다면, 이 값을 확인해 자동 시작을 건너뛰어야 Shot Play가 선택한 Shot에서 바로 시작된다.

### Scene 시작 절차

1. `StorySceneRegistry`에서 `SceneID`에 대응하는 `StorySceneAsset`을 찾는다.
2. `TargetLevel`이 현재 레벨과 다르면 pending travel 절차를 시작한다.
3. `LoadingLevel`로 이동한다.
4. `TargetLevel`을 async load한다.
5. 최소 로딩 시간 조건을 만족하면 TargetLevel을 연다.
6. TargetLevel 진입 후 SceneTemplate을 실행한다.
7. Entry 링크 또는 지정 Shot을 시작한다.

### Shot 진행

1. `MoveToShot(ShotID)`
2. ShotNode에서 `ShotTemplate` 확인
3. `UStoryShotBase` 인스턴스 생성
4. `InitializeShot`
5. `EnterShot`
6. 매 Tick에서 `TickShot`
7. `FinishShot` 호출 즉시 `ExitShot` 및 다음 링크 처리

### Branch 진행

1. Shot의 NextLink가 Branch면 `EvaluateBranch` 실행
2. BranchTemplate의 Outputs 개수를 확인
3. BranchTemplate 인스턴스 생성
4. `InitializeBranch(CurrentRef)`
5. Outputs가 2개 이상이면 `SelectNextIndex(OutputCount)` 호출
6. 반환 인덱스를 안전 범위로 clamp
7. 해당 출력 핀 링크로 Shot 또는 Scene 전이

Branch는 저장 가능한 지속 상태가 아니라 순간 판단 단계로 취급한다.

### Transition 진행

Transition 노드는 런타임 인스턴스가 없다. Compile 시 `NextSceneID` 링크로 변환되고, 실행 중 해당 링크를 만나면 `StartFromScene(NextSceneID)`로 전이한다.

---

## 에디터 기능

### 생성 가능한 에셋/Blueprint

Content Browser의 `StoryFlow` 카테고리에서 다음을 생성할 수 있다.

- `StoryFlow Scene Asset` (`UStorySceneAsset`)
- `StoryFlow Scene` (`UStorySceneBase`)
- `StoryFlow Shot` (`UStoryShotBase`)
- `StoryFlow Branch` (`UStoryBranchBase`)

### Story Scene Editor

`StoryFlow Scene Asset`을 열면 전용 에디터가 열린다.

구성:

- Graph 탭
- Details 탭
- Compile 버튼

지원 기능:

- Shot / Branch / Transition 노드 생성
- Copy / Cut / Paste / Delete
- Undo / Redo
- `C` 키로 comment box 생성
- Compile 상태 표시
- PIE 전 자동 Compile/검증 및 실패 시 PIE 차단
- Shot 노드 Play 버튼으로 해당 Shot부터 PIE 시작
  - Editor Play From Shot 세션 동안 `UStoryFlowSubsystem::IsEditorPlayFromShotSession()`이 true가 되어 프로젝트의 자동 StoryFlow 시작 로직이 이를 스킵할 수 있다
- Entry / Shot / Branch 노드 더블클릭으로 연결된 Template 빠르게 열기
  - Entry: SceneTemplate
  - Shot: ShotTemplate
  - Branch: BranchTemplate
  - Blueprint 기반 Template은 Blueprint 에디터로 열고, C++ 네이티브 Template은 Source Code Accessor 설정에 따라 IDE의 해당 클래스 소스로 이동

### 그래프 노드 표시 정책

| 노드 | 제목 | 설명 표시 | 색상 |
| --- | --- | --- | --- |
| Entry | `Entry` + 선택적 Scene DisplayName | Scene Description을 comment bubble로 표시 | 녹색 계열 |
| Shot | `Shot` + 선택적 DisplayName | Description을 comment bubble로 표시 | 파란색 계열 |
| Branch | `Branch` + 선택적 DisplayName | Description을 comment bubble로 표시 | 주황색 계열 |
| Transition | `Transition` + NextSceneID | Description을 comment bubble로 표시 | 보라색 계열 |

Entry는 SceneAsset의 DisplayName을 제목 아래에 표시한다. ShotID/BranchID 같은 내부 ID는 노드 제목에서 숨긴다. Transition은 목적지가 핵심 정보이므로 `NextSceneID`를 제목 아래에 표시한다.

Details에서 DisplayName 또는 Description을 수정하면 노드 제목/comment bubble은 Compile을 기다리지 않고 즉시 갱신된다.

### 연결 규칙

허용:

- `Entry -> Shot`
- `Entry -> Branch`
- `Entry -> Transition`
- `Shot -> Shot`
- `Shot -> Branch`
- `Shot -> Transition`
- `Branch -> Shot`
- `Branch -> Branch`
- `Branch -> Transition`

금지:

- `Transition -> *`
- 같은 방향 핀 연결
- StoryFlow 핀이 아닌 핀 연결

추가 규칙:

- 출력 핀은 1개 연결만 허용한다.
- 입력 핀은 여러 연결을 허용한다. Branch `In` 핀도 Entry/Shot/Branch 출력에서 오는 여러 연결을 받을 수 있다.
- Branch 출력 핀은 `Next_0`, `Next_1` 순서로 정렬/사용한다.
- BranchTemplate이 없거나 Outputs가 비어 있으면 Branch 출력 핀을 표시하지 않는다.
- Branch 출력 핀의 내부 이름은 `Next_i`를 유지하고, 그래프 표시명은 Outputs의 DisplayName을 사용한다.

---

## Compile 동작

Compile은 에디터 그래프를 런타임 데이터로 접어 넣는 과정이다.

주요 순서:

1. ShotID 자동 보정
2. Entry/Shot/Branch/Transition Description을 comment bubble로 재동기화
3. Entry에서 도달 가능한 노드 집합 계산
4. 런타임 데이터 재생성
   - Entry `_EntryLink`
   - Shot `_NextLink`
   - Branch `_NextLinksByPinIndex`
5. 무결성 검사
6. 성공/실패 상태와 노드별 오류 메시지 갱신

중요:

- Entry에서 도달 불가능한 노드는 Compile/런타임 경로에서 제외된다.
- 미연결 작업용 노드는 그래프에 남겨둘 수 있지만 실행되지 않는다.

### 주요 검증 항목

Scene:

- `SceneID` 유효성
- `StorySceneRegistry` 설정 여부
- `LoadingLevel` 설정 여부
- `TargetLevel` 설정 여부
- `LoadingLevel != TargetLevel`
- `SceneTemplate` 설정 여부
- Registry에 Scene 등록 여부
- Registry 내 SceneID 중복 여부
- Entry가 Shot/Branch/Transition 중 하나에 연결되었는지
- Entry 링크가 실제 Shot/Branch 또는 Registry에 등록된 Scene을 가리키는지

Shot:

- `ShotID` 유효성/중복
- `ShotTemplate` 설정 여부
- `NextLink`가 실존 Shot/Branch 또는 Registry에 등록된 Scene을 가리키는지

Branch:

- `BranchID` 유효성/중복
- `BranchTemplate` 설정 여부
- BranchTemplate Outputs가 비어 있지 않은지
- 각 출력 핀이 연결되었는지
- 각 출력 링크가 실존 Shot 또는 Registry에 등록된 Scene을 가리키는지

Transition:

- 연결된 Transition의 `NextSceneID` 유효성
- Registry 등록 여부
- Registry 내 중복 여부

---

## 디버깅

콘솔 명령:

```text
Debug StoryFlow
```

동작:

- 한 번 입력하면 StoryFlow 디버그 오버레이 On
- 다시 입력하면 Off
- 화면 좌측 하단에 표시

현재 오버레이 정보:

- `SceneID`
- `ShotID`
- `NextLink`
- `TravelPhase`
- `PendingStart`
- `PendingTargetLevel`
- `TargetLoadProgress`

색상 정책:

- Key: 흰색
- 일반 Value: 청록색
- SceneID/ShotID 값: 노란색

---

## 저장 / 복원 정책

저장 대상:

- `SceneID`
- `ShotID`

권장 저장 시점:

- Shot이 진행 중일 때 `GetCurrentRef()`로 저장

복원:

- `StartFromRef(FStoryFlowRef)` 호출

Branch는 순간 판단 단계이므로 저장 대상으로 보지 않는다. Branch 중간 상태를 저장하려 하지 말고, 마지막으로 진입한 Shot 또는 다음으로 확정된 Shot 기준으로 저장하는 것이 맞다.

---

## 현재 한계

- 로딩 UI 위젯 자체는 플러그인에서 제공하지 않는다.
- 저장/복원은 의도적으로 `SceneID + ShotID` 단위다.
- Branch의 선택 결과나 중간 상태는 저장하지 않는다.
- Shot은 직접 다중 출력하지 않는다. 다중 흐름은 Branch 노드로 표현한다.

---

## 추천 코드 읽기 순서

1. `StoryFlowDefines.h`
2. `StorySceneBase.h`
3. `StoryShotBase.h`
4. `StoryBranchBase.h`
5. `StorySceneNodeData.h`
6. `StoryBranchNodeData.h`
7. `StorySceneAsset.h`
8. `StorySceneRegistryAsset.h`
9. `StoryFlowDeveloperSettings.h`
10. `StoryFlowSubsystem.h/.cpp`
11. `StoryFlow.cpp`
12. `StoryFlowEditor.cpp`
13. `StorySceneEditor.cpp`
14. `StorySceneEdGraph.cpp`
15. `StorySceneGraphSchema.cpp`
16. `StorySceneGraphNode_*.cpp`

---

## 관련 문서

- [User Guide](./UserGuide.md) — 플러그인을 사용하는 디자이너/개발자용 작업 가이드
