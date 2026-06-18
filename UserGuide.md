# StoryFlow User Guide

이 문서는 StoryFlow 플러그인을 실제 프로젝트에서 사용하는 사람을 위한 작업 가이드다. 코드 구조 설명보다 **무엇을 만들고, 어디에 설정하고, 어떻게 실행/검증하는지**에 초점을 둔다.

---

## 1. 기본 개념

StoryFlow는 다음 단위로 스토리를 구성한다.

| 이름 | 설명 |
| --- | --- |
| Scene | 장소/상황 단위. 하나의 `StorySceneAsset`으로 만든다. |
| Shot | Scene 안에서 실제로 실행되는 진행 단계. |
| Branch | 조건에 따라 다음 Shot 또는 다음 Scene을 선택하는 분기 노드. |
| Transition | 다른 Scene으로 넘어가는 노드. |
| Registry | SceneID로 SceneAsset을 찾기 위한 목록. |

가장 단순한 흐름은 다음과 같다.

```text
Entry -> Shot -> Shot -> Transition
```

분기가 필요하면 다음처럼 만든다.

```text
Entry -> Shot -> Branch -> Shot
                    └----> Transition
```

---

## 2. 처음 설정하기

### 2.1 Registry 만들기

1. Content Browser에서 `Data Asset`을 생성한다.
2. 클래스 선택 창에서 `StorySceneRegistryAsset`을 선택한다.
3. 프로젝트에서 사용할 `StorySceneAsset`들을 Registry의 Scenes 배열에 추가한다.

Registry는 런타임에서 `SceneID`로 실제 SceneAsset을 찾는 데 필요하다.

### 2.2 Project Settings 설정

Project Settings에서 StoryFlow 설정을 찾고 다음을 지정한다.

| 설정 | 필수 | 설명 |
| --- | --- | --- |
| StorySceneRegistry | 필수 | 방금 만든 Registry Asset |
| LoadingLevel | 필수 | Scene의 TargetLevel과 다른 로딩 전용 레벨 |
| MinimumLoadingLevelDuration | 선택 | 로딩 화면을 최소 몇 초 유지할지 |
| MinimumLoadingLevelProgressCurve | 선택 | 로딩 진행률 표시 보정 커브 |

주의:

- `LoadingLevel`과 각 Scene의 `TargetLevel`은 서로 달라야 한다.
- Loading UI는 플러그인이 자동으로 만들어주지 않는다. LoadingLevel 안에서 프로젝트 UI로 구현한다.

---

## 3. 실행 로직 Blueprint 만들기

Content Browser의 `StoryFlow` 카테고리에서 다음 Blueprint를 만든다.

### 3.1 Scene Blueprint

부모 클래스: `StorySceneBase`

사용할 이벤트:

- `OnEnterScene`
- `OnExitScene`

용도 예시:

- Scene 시작 BGM 재생
- Scene 전용 상태 초기화
- UI 표시
- Scene 종료 시 정리

### 3.2 Shot Blueprint

부모 클래스: `StoryShotBase`

사용할 이벤트/함수:

- `OnEnterShot`
- `OnTickShot`
- `OnExitShot`
- `FinishShot`

Shot은 `FinishShot`을 호출해야 다음 노드로 진행한다.

예시:

1. `OnEnterShot`에서 대사 UI 표시
2. 입력 또는 타이머를 기다림
3. 완료 조건이 되면 `FinishShot` 호출
4. Shot이 즉시 종료되고 NextLink로 진행

### 3.3 Branch Blueprint

부모 클래스: `StoryBranchBase`

사용할 이벤트:

- `SelectNextIndex(NextCount)`

`Branch Outputs` 배열은 Branch 노드의 출력 핀 개수와 표시명을 정의한다.

예:

- `Yes`
- `No`
- `Ignore`

반환값:

- `0`이면 `Next_0`
- `1`이면 `Next_1`
- ...

반환값이 범위를 벗어나도 런타임에서 안전 범위로 clamp된다. 그래도 명확한 조건을 작성하는 것이 좋다.

---

## 4. StoryFlow Scene Asset 만들기

1. Content Browser의 `StoryFlow` 카테고리에서 `StoryFlow Scene Asset`을 만든다.
2. 에셋 이름을 의미 있게 정한다.
3. Details에서 다음을 설정한다.

| 항목 | 설명 |
| --- | --- |
| SceneID | Registry와 런타임 시작에 쓰이는 고유 ID |
| DisplayName | 사람이 읽기 쉬운 이름. 비어 있으면 SceneTemplate 설정 시 템플릿 이름으로 자동 입력 |
| Description | Entry 노드 comment bubble로 표시되는 Scene 설명 |
| TargetLevel | 이 Scene이 실행될 레벨 |
| SceneTemplate | Scene Blueprint 인스턴스 |

새 StoryFlow Scene Asset은 기본적으로 에셋 이름을 SceneID로 사용한다.

---

## 5. 그래프 편집하기

StoryFlow Scene Asset을 더블클릭하면 Story Scene Editor가 열린다.

### 5.1 노드 만들기

그래프 우클릭 메뉴에서 생성한다.

- `Add Shot`
- `Add Branch`
- `Add Transition`

`C` 키를 누르면 comment box를 만들 수 있다.

### 5.2 Entry 노드

Entry 노드는 Scene 시작점을 나타내며 삭제하거나 복제할 수 없다.

표시:

- 노드 제목은 `Entry`이며, Scene Asset의 DisplayName이 있으면 아래 줄에 표시된다.
- Scene Asset의 DisplayName을 수정하면 Entry 노드 제목에 바로 반영된다.
- Scene Asset의 Description은 Entry 노드 comment bubble로 표시되며, 수정하면 바로 반영된다.
- Entry 노드를 선택하면 Details에는 Scene Asset이 표시된다.

### 5.3 Shot 노드

Shot 노드 Details에서 주로 편집하는 항목:

| 항목 | 설명 |
| --- | --- |
| ShotID | 자동 생성, 읽기 전용 |
| DisplayName | 노드 제목 아래에 표시되는 이름. 비어 있으면 ShotTemplate 설정 시 템플릿 이름으로 자동 입력 |
| Description | 노드 comment bubble로 표시되는 설명 |
| ShotTemplate | 실행할 Shot Blueprint 인스턴스 |

노드 제목은 항상 `Shot`이며, DisplayName이 있으면 아래 줄에 표시된다.
DisplayName을 수정하면 Shot 노드 제목에 바로 반영된다.
Description을 수정하면 Shot 노드 comment bubble에 바로 반영된다.

### 5.4 Branch 노드

Branch 노드 Details에서 주로 편집하는 항목:

| 항목 | 설명 |
| --- | --- |
| DisplayName | 노드 제목 아래에 표시되는 이름. 비어 있으면 BranchTemplate 설정 시 템플릿 이름으로 자동 입력 |
| Description | 노드 comment bubble로 표시되는 설명 |
| BranchTemplate | 실행할 Branch Blueprint 인스턴스. Outputs가 출력 핀 개수와 표시명을 정의 |

BranchID는 자동 관리되며 Details에 표시하지 않는다. 노드 제목은 `Branch`이며, DisplayName이 있으면 아래 줄에 표시된다.
DisplayName을 수정하면 Branch 노드 제목에 바로 반영된다.
Description을 수정하면 Branch 노드 comment bubble에 바로 반영된다.

BranchTemplate이 없거나 Outputs가 비어 있으면 출력 핀은 표시되지 않는다.
Outputs를 설정하면 `Next_0`, `Next_1`, ... 출력 핀이 생기고, 핀 표시명은 각 Output의 DisplayName을 사용한다.

### 5.5 Transition 노드

Transition 노드 Details에서 편집하는 항목:

| 항목 | 설명 |
| --- | --- |
| NextSceneID | 이동할 다음 SceneID |
| Description | 노드 comment bubble로 표시되는 설명 |

Transition 노드 제목은 `Transition`이며, NextSceneID가 있으면 아래 줄에 표시된다.
NextSceneID를 수정하면 Transition 노드 제목에 바로 반영된다.
Description을 수정하면 Transition 노드 comment bubble에 바로 반영된다.

### 5.6 Template 빠르게 열기

Story Scene Editor에서 주요 노드를 더블클릭하면 해당 노드가 사용하는 Template을 바로 열 수 있다.

| 노드 | 더블클릭 시 열리는 대상 |
| --- | --- |
| Entry | Scene Asset의 `SceneTemplate` |
| Shot | Shot 노드의 `ShotTemplate` |
| Branch | Branch 노드의 `BranchTemplate` |

동작:

- Blueprint 기반 Template은 Blueprint 에디터로 열린다.
- C++ 네이티브 Template은 Unreal Editor의 Source Code Accessor 설정에 따라 Visual Studio/Rider 등 IDE에서 해당 클래스 소스로 이동한다.
- Template이 비어 있으면 아무 동작도 하지 않는다.
- Template은 있지만 에디터/소스 이동 대상을 찾지 못하면 경고 알림이 표시된다.

### 5.7 연결 규칙

가능한 연결:

```text
Entry -> Shot
Entry -> Branch
Entry -> Transition
Shot -> Shot
Shot -> Branch
Shot -> Transition
Branch -> Shot
Branch -> Branch
Branch -> Transition
```

불가능한 연결:

```text
Transition -> Any
```

출력 핀은 1개 연결만 가진다. 새 출력 연결을 만들면 기존 연결은 자동으로 교체될 수 있다.
입력 핀은 여러 연결을 받을 수 있다. Branch의 `In` 핀도 여러 Entry/Shot/Branch 출력에서 동시에 들어오는 연결을 받을 수 있다.

---

## 6. Compile하기

그래프를 수정한 뒤에는 상단의 `Compile` 버튼을 누른다.

Compile이 하는 일:

- Entry에서 도달 가능한 노드만 런타임 데이터로 변환
- Entry 시작 링크 갱신
- Shot/Branch의 다음 링크 갱신
- Entry/Shot/Branch/Transition Description을 comment bubble로 재동기화
- 필수 설정 누락/잘못된 연결 검사

Compile 실패 시:

- Compile 버튼 상태가 Error로 바뀐다.
- 문제가 있는 노드 하단에 빨간 오류가 표시된다.
- PIE 시작이 차단된다.

자주 나는 오류:

| 오류 상황 | 해결 |
| --- | --- |
| Entry가 Shot에 연결되지 않음 | Entry의 Next를 첫 Shot에 연결 |
| ShotTemplate 누락 | 각 Shot에 Shot Blueprint 인스턴스 지정 |
| BranchTemplate 누락 | Branch에 Branch Blueprint 인스턴스 지정 |
| Branch 출력 핀 미연결 | 모든 `Next_*` 핀 연결 |
| NextSceneID가 Registry에 없음 | Registry에 대상 SceneAsset 추가 |
| LoadingLevel과 TargetLevel이 같음 | 서로 다른 레벨로 설정 |

---

## 7. 실행하기

### 7.1 일반 시작

Blueprint나 C++에서 `StoryFlowSubsystem`을 얻어 시작한다.

Blueprint 기준:

1. GameInstance에서 `Get Subsystem`으로 `StoryFlowSubsystem`을 가져온다.
2. `StartFromScene`을 호출한다.
3. 시작할 `SceneID`를 넘긴다.

C++ 예시:

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

자동 시작은 `BeginPlay`보다 `GameMode::StartPlay()`의 `Super::StartPlay()` 이후에 두는 것을 권장한다. 그러면 PlayerController와 월드 Actor의 BeginPlay가 먼저 끝난 뒤 첫 Shot의 `OnEnterShot`이 실행된다.

### 7.2 특정 Shot부터 테스트

Story Scene Editor에서 Shot 노드의 Play 버튼을 누르면 PIE가 시작되고 해당 Shot부터 StoryFlow가 실행된다.

주의:

- PIE 중에는 버튼이 숨겨진다.
- Compile 오류가 있으면 PIE가 차단된다.
- Shot Play로 시작한 PIE에서는 `UStoryFlowSubsystem::IsEditorPlayFromShotSession()`이 true다. 프로젝트가 일반 PIE 시작 시 Intro Scene을 자동 실행한다면 이 값을 확인해 자동 시작을 건너뛰어야 한다.

### 7.3 중단

`StopScene`을 호출하면 현재 Scene/Shot/Branch 상태와 pending travel 상태가 정리된다.

---

## 8. 저장 / 복원

저장할 값:

- `SceneID`
- `ShotID`

현재 진행 상태는 `GetCurrentRef()`로 얻는다.

```cpp
FStoryFlowRef SaveRef = StoryFlow->GetCurrentRef();
```

복원은 `StartFromRef`를 사용한다.

```cpp
StoryFlow->StartFromRef(SaveRef);
```

권장:

- Shot이 진행 중일 때 저장한다.
- Branch는 순간 판단 단계라 저장 대상으로 보지 않는다.

---

## 9. 로딩 화면 만들기

StoryFlow는 TargetLevel 이동 전에 LoadingLevel로 진입하고 TargetLevel을 비동기로 로드한다.

LoadingLevel에서 프로젝트 UI가 다음 값을 읽어 progress bar를 표시할 수 있다.

```cpp
float Progress = StoryFlow->GetTargetLevelLoadingProgressRate();
```

값 범위:

- `0.0` ~ `1.0`

MinimumLoadingLevelDuration과 ProgressCurve를 설정하면 표시용 진행률이 보정된다.

---

## 10. 디버깅

콘솔에서 다음 명령을 입력한다.

```text
Debug StoryFlow
```

한 번 입력하면 켜지고, 다시 입력하면 꺼진다.

화면 좌측 하단에 표시되는 정보:

- `SceneID`
- `ShotID`
- `NextLink`
- `TravelPhase`
- `PendingStart`
- `PendingTargetLevel`
- `TargetLoadProgress`

색상:

- Key는 흰색
- 일반 값은 청록색
- SceneID/ShotID 값은 노란색

---

## 11. 추천 작업 순서

새 Scene을 만들 때는 이 순서가 가장 안전하다.

1. Scene/Shot/Branch Blueprint를 먼저 만든다.
2. StorySceneAsset을 만든다.
3. SceneID, TargetLevel, SceneTemplate을 설정한다.
4. Registry에 StorySceneAsset을 등록한다.
5. 그래프에서 Entry -> 첫 Shot을 연결한다.
6. 각 Shot에 ShotTemplate과 Description을 입력한다.
7. 분기가 필요하면 Branch 노드와 BranchTemplate을 추가한다.
8. 다음 Scene이 필요하면 Transition 노드와 NextSceneID를 설정한다.
9. Compile한다.
10. Shot Play 버튼 또는 `StartFromScene`으로 테스트한다.
11. 필요하면 `Debug StoryFlow`로 런타임 상태를 확인한다.

---

## 12. 운영 팁

- Shot은 작게 유지한다. 대사 한 덩어리, 연출 한 단계처럼 끝나는 단위가 좋다.
- 여러 선택지는 Shot에서 처리하지 말고 Branch로 분리한다.
- Transition은 Scene 간 이동에만 사용한다.
- DisplayName은 사람이 읽기 좋은 이름, ID는 시스템 식별자로 생각한다.
- Description은 그래프에서 기획 의도를 공유하는 메모로 적극 활용한다.
- 미연결 노드는 실행되지 않으므로 임시 작업 노드를 그래프에 남겨둘 수 있다.
- Compile 오류가 난 상태에서는 PIE가 막히는 것이 정상이다.
