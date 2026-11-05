// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "StorySceneBuildLibrary.generated.h"

class UStorySceneAsset;
class UStoryShotBase;

/* RebuildLinearSceneGraph에 넘기는 Shot 한 개의 설정. */
USTRUCT(BlueprintType)
struct STORYFLOWEDITOR_API FStorySceneBuildShot
{
	GENERATED_BODY()

public:
	/* 이 Scene 안에서 겹치지 않는 ShotID (예: S01_010). 이어하기 저장에 그대로 쓰인다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StoryFlow")
	FName ShotID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StoryFlow")
	TSubclassOf<UStoryShotBase> ShotClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StoryFlow")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StoryFlow")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StoryFlow")
	bool IsCheckpoint = false;
};

/*
 * 에디터 스크립트(Python 등)로 StoryScene을 만들 때 쓰는 함수 모음.
 * 그래프 노드와 런타임 데이터를 에디터에서 손으로 만든 것과 같은 형태로 만든다. 에셋 에디터 창은 열지 않는다.
 */
UCLASS()
class STORYFLOWEDITOR_API UStorySceneBuildLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/* StorySceneAsset을 새로 만든다. 같은 경로에 이미 있으면 그 에셋을 돌려준다. 저장은 하지 않는다. */
	UFUNCTION(BlueprintCallable, Category = "StoryFlow|Editor")
	static UStorySceneAsset* CreateSceneAsset(const FString& _package_path, const FString& _asset_name, FName _scene_id);

	/*
	 * Scene 그래프를 _shots 순서대로 한 줄로 다시 만든다. Entry를 뺀 기존 노드(Shot/Branch/Transition)는 모두 지운다.
	 * _next_scene_id가 있으면 마지막 Shot 뒤에 그 Scene으로 가는 Transition을 붙인다.
	 * 만든 Shot 템플릿을 순서대로 돌려주므로, 호출한 쪽에서 각 템플릿의 속성을 채우면 된다. 실패하면 빈 배열.
	 */
	UFUNCTION(BlueprintCallable, Category = "StoryFlow|Editor")
	static TArray<UStoryShotBase*> RebuildLinearSceneGraph(UStorySceneAsset* _scene_asset, const TArray<FStorySceneBuildShot>& _shots, FName _next_scene_id);
};
