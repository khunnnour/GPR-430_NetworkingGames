// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UMyGameInstance::UMyGameInstance()
{
}


void UMyGameInstance::Init()
{
	// checked if we can access the online subsystem
	if (IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get())
	{
		SessionInterface = SubSystem->GetSessionInterface(); // get the session interface
		if(SessionInterface.IsValid())
		{
			// bind delegate when session is created
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMyGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnJoinSessionComplete);
		}
	}
}

void UMyGameInstance::OnCreateSessionComplete(FName ServerName, bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete, Succeeded: %d"), Succeeded);
	if (Succeeded)
	{
		// switch to new map
		GetWorld()->ServerTravel("/Game/FirstPersonCPP/Maps/FirstPersonExampleMap?listen");
	}
}

void UMyGameInstance::OnFindSessionComplete(bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete, Succeeded: %d"), Succeeded);
	if (Succeeded)
	{
		TArray<FOnlineSessionSearchResult> SearchResults= SessionSearch->SearchResults; // get all results
		
		UE_LOG(LogTemp, Warning, TEXT("SearchResults, number: %d"), SearchResults.Num());

		if (SearchResults.Num() > 0)// if mutliple results
		{
			UE_LOG(LogTemp, Warning, TEXT("Joining Server"));
			SessionInterface->JoinSession(0, "My Session", SearchResults[0]); // should only be the one
		}
	}
}

void UMyGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type result)
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete, Session Name: %s"), *SessionName.ToString());
	
	// player index is 0 bc should be on main menu
	if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAddress="";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress); // get the address of the session
		if (JoinAddress != "")// check if address is empty
			PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
	}
}

// creates the session/server
void UMyGameInstance::CreateServer()
{
	UE_LOG(LogTemp, Warning, TEXT("CreateServer"));

	// create the session settings
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = 4;

	// create the actual session
	SessionInterface->CreateSession(0, FName("My Session"), SessionSettings);
}

void UMyGameInstance::JoinServer()
{
	// set search parameters
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true; // Is it LAN?
	SessionSearch->MaxSearchResults=10000; // Max results (high bc we use a public app id so a lot of things can show up)
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	// search for sessions
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}
