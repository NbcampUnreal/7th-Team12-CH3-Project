// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "TheSeventhbullet/TheSeventhbulletGameMode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTheSeventhbulletGameMode() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
THESEVENTHBULLET_API UClass* Z_Construct_UClass_ATheSeventhbulletGameMode();
THESEVENTHBULLET_API UClass* Z_Construct_UClass_ATheSeventhbulletGameMode_NoRegister();
UPackage* Z_Construct_UPackage__Script_TheSeventhbullet();
// End Cross Module References

// Begin Class ATheSeventhbulletGameMode
void ATheSeventhbulletGameMode::StaticRegisterNativesATheSeventhbulletGameMode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ATheSeventhbulletGameMode);
UClass* Z_Construct_UClass_ATheSeventhbulletGameMode_NoRegister()
{
	return ATheSeventhbulletGameMode::StaticClass();
}
struct Z_Construct_UClass_ATheSeventhbulletGameMode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering HLOD WorldPartition DataLayers Transformation" },
		{ "IncludePath", "TheSeventhbulletGameMode.h" },
		{ "ModuleRelativePath", "TheSeventhbulletGameMode.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ATheSeventhbulletGameMode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_ATheSeventhbulletGameMode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AGameModeBase,
	(UObject* (*)())Z_Construct_UPackage__Script_TheSeventhbullet,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ATheSeventhbulletGameMode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ATheSeventhbulletGameMode_Statics::ClassParams = {
	&ATheSeventhbulletGameMode::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x008802ACu,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ATheSeventhbulletGameMode_Statics::Class_MetaDataParams), Z_Construct_UClass_ATheSeventhbulletGameMode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ATheSeventhbulletGameMode()
{
	if (!Z_Registration_Info_UClass_ATheSeventhbulletGameMode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ATheSeventhbulletGameMode.OuterSingleton, Z_Construct_UClass_ATheSeventhbulletGameMode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ATheSeventhbulletGameMode.OuterSingleton;
}
template<> THESEVENTHBULLET_API UClass* StaticClass<ATheSeventhbulletGameMode>()
{
	return ATheSeventhbulletGameMode::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ATheSeventhbulletGameMode);
ATheSeventhbulletGameMode::~ATheSeventhbulletGameMode() {}
// End Class ATheSeventhbulletGameMode

// Begin Registration
struct Z_CompiledInDeferFile_FID_TheSeventhbullet_Source_TheSeventhbullet_TheSeventhbulletGameMode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ATheSeventhbulletGameMode, ATheSeventhbulletGameMode::StaticClass, TEXT("ATheSeventhbulletGameMode"), &Z_Registration_Info_UClass_ATheSeventhbulletGameMode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ATheSeventhbulletGameMode), 908480121U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_TheSeventhbullet_Source_TheSeventhbullet_TheSeventhbulletGameMode_h_203969647(TEXT("/Script/TheSeventhbullet"),
	Z_CompiledInDeferFile_FID_TheSeventhbullet_Source_TheSeventhbullet_TheSeventhbulletGameMode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_TheSeventhbullet_Source_TheSeventhbullet_TheSeventhbulletGameMode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
