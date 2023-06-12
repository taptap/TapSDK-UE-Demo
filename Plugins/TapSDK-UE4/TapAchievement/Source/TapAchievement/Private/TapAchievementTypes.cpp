// Copyright Epic Games, Inc. All Rights Reserved.

#include "TapAchievementTypes.h"
#include "TUType.h"

#define LOCTEXT_NAMESPACE "TapAchievement"

FAchievementDescTap::FAchievementDescTap(const TSharedPtr<FJsonObject>& JsonObject)
{
	JsonObject->TryGetStringField(TEXT("achievement_id"), AchievementId);
	JsonObject->TryGetStringField(TEXT("achievement_open_id"), DisplayId);
	JsonObject->TryGetBoolField(TEXT("is_hide"), bIsHide);
	JsonObject->TryGetNumberField(TEXT("count_step"), CountStep);
	JsonObject->TryGetNumberField(TEXT("show_order"), ShowOrder);

	const TSharedPtr<FJsonObject>* ObjP = nullptr;
	if (JsonObject->TryGetObjectField(TEXT("achievement_config_out_dto"), ObjP))
	{
		(*ObjP)->TryGetStringField(TEXT("achievement_title"), AchievementTitle);
		(*ObjP)->TryGetStringField(TEXT("achievement_sub_title"), AchievementSubtitle);
		(*ObjP)->TryGetStringField(TEXT("achievement_icon"), AchievementIconUrl);
	}
	
	if (JsonObject->TryGetObjectField(TEXT("achievement_rarity"), ObjP))
	{
		(*ObjP)->TryGetNumberField(TEXT("rarity"), RarityD);
		Rarity = RarityD;
		
		(*ObjP)->TryGetNumberField(TEXT("level"), Level);
	}

	JsonObject->TryGetNumberField(TEXT("achievement_type"), Type);
}


FAchievementTap::FAchievementTap(const TSharedPtr<FJsonObject>& JsonObject)
{
	JsonObject->TryGetStringField(TEXT("achievement_id"), AchievementId);
	JsonObject->TryGetStringField(TEXT("achievement_open_id"), DisplayId);
	int64 Ticks;
	JsonObject->TryGetNumberField(TEXT("complete_time"), Ticks);
	CompleteTime = FDateTime::FromUnixTimestamp(Ticks / 1000);
	JsonObject->TryGetNumberField(TEXT("completed_step"), CompleteStep);
	JsonObject->TryGetNumberField(TEXT("completed_step"), CompleteStep);
	JsonObject->TryGetBoolField(TEXT("full_completed"), bFullCompleted);
}

FAchievementTap::FAchievementTap(const FAchievementDescTap& Desc)
{
	AchievementId = Desc.AchievementId;
	DisplayId = Desc.DisplayId;
}

void FAchievementTap::Write(const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>>& Writer) const
{
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("achievement_id"), AchievementId);
	Writer->WriteValue(TEXT("achievement_open_id"), DisplayId);
	Writer->WriteValue(TEXT("completed_step"), CompleteStep);
	const int64 Ticks = CompleteTime.ToUnixTimestamp() * 1000;
	Writer->WriteValue(TEXT("complete_time"), Ticks);
	Writer->WriteObjectEnd();
}

bool FAchievementTap::UpdateStep(const FAchievementDescTap& Description, int32 NewStep)
{
	int32 Step = FMath::Clamp(NewStep, CompleteStep, Description.CountStep);//目前不支持倒退
	if (CompleteStep != Step)
	{
		CompleteStep = Step;
		bFullCompleted = CompleteStep == Description.CountStep;
		CompleteTime = FDateTime::UtcNow();
		return true;
	}
	return false;
}

void FAchievementLanguage::UpdateLanguage(ELanguageType NewType)
{
	switch (NewType)
	{
	case ELanguageType::ZH:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "已解锁{0}项成就，共{1}项成就");
		DialogPercent = LOCTEXT("RarityFormat", "“有{0}%的玩家达成此项成就”");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "“只有不到0.1%的玩家达成此项成就”");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- {0}.{1}.{2} 获得 -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "隐藏");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "继续玩下去就会知道了");
		Unlocked = LOCTEXT("AchievementUnlocked", "已解锁");
		Locked = LOCTEXT("AchievementLocked", "未解锁");
		RarityCommon = LOCTEXT("AchievementCommon", "普通");
		RarityUncommon = LOCTEXT("AchievementUncommon", "稀有");
		RarityRare = LOCTEXT("AchievementRare", "珍贵");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "极为珍贵");
		PopTitle = LOCTEXT("AchievementPopTitle", "恭喜解锁新成就！");
		break;
	case ELanguageType::EN:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}/{1} Achievements");
		DialogPercent = LOCTEXT("RarityFormat", "'{0}% of players have this'");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "'Fewer than 0.1% of players have achieved this'");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- Accomplished on {0}.{1}.{2} -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "Hidden");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "Keep playing to find out");
		Unlocked = LOCTEXT("AchievementUnlocked", "UNLOCKED");
		Locked = LOCTEXT("AchievementLocked", "LOCKED");
		RarityCommon = LOCTEXT("AchievementCommon", "Common");
		RarityUncommon = LOCTEXT("AchievementUncommon", "Rare");
		RarityRare = LOCTEXT("AchievementRare", "Super Rare");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "Ultra Rare");
		PopTitle = LOCTEXT("AchievementPopTitle", "Achievement unlocked!");
		break;
	case ELanguageType::ZHTW:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "已解鎖{0}項成就，共{1}項成就");
		DialogPercent = LOCTEXT("RarityFormat", "“有{0}%的玩家達成此項成就”");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "“只有不到0.1%的玩家達成此項成就”");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- {0}.{1}.{2} 獲得 -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "隱藏");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "繼續玩下去就會知道囉");
		Unlocked = LOCTEXT("AchievementUnlocked", "已解鎖");
		Locked = LOCTEXT("AchievementLocked", "未解鎖");
		RarityCommon = LOCTEXT("AchievementCommon", "普通");
		RarityUncommon = LOCTEXT("AchievementUncommon", "稀有");
		RarityRare = LOCTEXT("AchievementRare", "珍貴");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "極為珍貴");
		PopTitle = LOCTEXT("AchievementPopTitle", "恭喜解鎖新成就！");
		break;
	case ELanguageType::JA:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}/{1} 達成項目");
		DialogPercent = LOCTEXT("RarityFormat", "“{0}%のプレイヤーがこの項目を達成しました”");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "“この項目を達成したプレイヤーは0.1％未満です”");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- 獲得：{0}.{1}.{2} -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "非表示");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "もう少しプレイしたら明らかになりますよ");
		Unlocked = LOCTEXT("AchievementUnlocked", "完了");
		Locked = LOCTEXT("AchievementLocked", "ロック中");
		RarityCommon = LOCTEXT("AchievementCommon", "ノーマル");
		RarityUncommon = LOCTEXT("AchievementUncommon", "ハード");
		RarityRare = LOCTEXT("AchievementRare", "スーパーハード");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "エクストリーム");
		PopTitle = LOCTEXT("AchievementPopTitle", "項目達成おめでとう！");
		break;
	case ELanguageType::KO:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}업적중 {1}개 의 업적을 달성 하였습니다");
		DialogPercent = LOCTEXT("RarityFormat", "“{0}%유저가 이 업적을 달성 하였습니다”");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "“0.1% 미만의 유저가 이 성과를 달성 하였습니다”");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- 획득: {0}.{1}.{2} -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "숨김");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "계속해서 플레이를 하면 알 수 있습니다");
		Unlocked = LOCTEXT("AchievementUnlocked", "해제");
		Locked = LOCTEXT("AchievementLocked", "미해제");
		RarityCommon = LOCTEXT("AchievementCommon", "노멀");
		RarityUncommon = LOCTEXT("AchievementUncommon", "희귀");
		RarityRare = LOCTEXT("AchievementRare", "유니크");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "울트라 유니크");
		PopTitle = LOCTEXT("AchievementPopTitle", "축하합니다. 새로운 업적을 달성 하였습니다.");
		break;
	case ELanguageType::TH:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}/{1} ความสำเร็จ");
		DialogPercent = LOCTEXT("RarityFormat", "“{0}% ของผู้เล่นได้รับความสำเร็จนี”");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "“ผู้เล่นน้อยกว่า 0.1% ได้รับความสำเร็จนี้”");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- สำเร็จเมื่อ {0}.{1}.{2} -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "ความสำเร็จลับ");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "เล่นเพื่อปลดล็อกความสำเร็จลับ");
		Unlocked = LOCTEXT("AchievementUnlocked", "ปลดล็อก");
		Locked = LOCTEXT("AchievementLocked", "ล็อก");
		RarityCommon = LOCTEXT("AchievementCommon", "คอมมอน");
		RarityUncommon = LOCTEXT("AchievementUncommon", "แรร์");
		RarityRare = LOCTEXT("AchievementRare", "ซุปเปอร์แรร์");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "อัลตราแรร์");
		PopTitle = LOCTEXT("AchievementPopTitle", "ปลดล็อกความสำเร็จ!");
		break;
	case ELanguageType::ID:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}/{1} Achievement");
		DialogPercent = LOCTEXT("RarityFormat", "“{0}% pemain mendapat Achievement ini”");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "“Kurang dari 0.1% pemain memiliki achievement ini”");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- Mendapat achievement pada {0}.{1}.{2} -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "Tersembunyi");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "Tetaplah bermain untuk membukanya");
		Unlocked = LOCTEXT("AchievementUnlocked", "UNLOCKED");
		Locked = LOCTEXT("AchievementLocked", "LOCKED");
		RarityCommon = LOCTEXT("AchievementCommon", "Common");
		RarityUncommon = LOCTEXT("AchievementUncommon", "Rare");
		RarityRare = LOCTEXT("AchievementRare", "Super Rare");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "Ultra Rare");
		PopTitle = LOCTEXT("AchievementPopTitle", "Mendapatkan achievement!");
		break;
	default: ;
	}
}

FAchievementLanguage* GAchievementLanguage = nullptr;

#undef LOCTEXT_NAMESPACE
