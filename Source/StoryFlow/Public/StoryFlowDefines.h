// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "StoryFlowDefines.generated.h"

USTRUCT(BlueprintType)
struct STORYFLOW_API FStorySceneID
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ID = NAME_None;

public:
	FStorySceneID() = default;

	explicit FStorySceneID(FName _id)
		: ID(_id)
	{
	}

	bool IsValid() const
	{
		return ID.IsNone() == false;
	}

	void Reset()
	{
		ID = NAME_None;
	}

	const FName& Get() const
	{
		return ID;
	}

	bool operator==(const FStorySceneID& _other) const
	{
		return ID == _other.ID;
	}

	bool operator!=(const FStorySceneID& _other) const
	{
		return ID != _other.ID;
	}
};

FORCEINLINE uint32 GetTypeHash(const FStorySceneID& _id)
{
	return GetTypeHash(_id.ID);
}

USTRUCT(BlueprintType)
struct STORYFLOW_API FStoryShotID
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ID = NAME_None;

public:
	FStoryShotID() = default;

	explicit FStoryShotID(FName _id)
		: ID(_id)
	{
	}

	bool IsValid() const
	{
		return ID.IsNone() == false;
	}

	void Reset()
	{
		ID = NAME_None;
	}

	const FName& Get() const
	{
		return ID;
	}

	bool operator==(const FStoryShotID& _other) const
	{
		return ID == _other.ID;
	}

	bool operator!=(const FStoryShotID& _other) const
	{
		return ID != _other.ID;
	}
};

FORCEINLINE uint32 GetTypeHash(const FStoryShotID& _id)
{
	return GetTypeHash(_id.ID);
}

USTRUCT(BlueprintType)
struct STORYFLOW_API FStoryBranchID
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ID = NAME_None;

public:
	FStoryBranchID() = default;

	explicit FStoryBranchID(FName _id)
		: ID(_id)
	{
	}

	bool IsValid() const
	{
		return ID.IsNone() == false;
	}

	void Reset()
	{
		ID = NAME_None;
	}

	const FName& Get() const
	{
		return ID;
	}

	bool operator==(const FStoryBranchID& _other) const
	{
		return ID == _other.ID;
	}

	bool operator!=(const FStoryBranchID& _other) const
	{
		return ID != _other.ID;
	}
};

FORCEINLINE uint32 GetTypeHash(const FStoryBranchID& _id)
{
	return GetTypeHash(_id.ID);
}

USTRUCT(BlueprintType)
struct STORYFLOW_API FStoryFlowRef
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FStorySceneID SceneID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FStoryShotID ShotID;

public:
	bool IsValid() const
	{
		return SceneID.IsValid();
	}

	void Reset()
	{
		SceneID.Reset();
		ShotID.Reset();
	}
};
