// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "EchoServer.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Fourtain.generated.h"

using namespace DHNet;

UCLASS()
class ARENABATTLE_API AFourtain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFourtain();
	virtual ~AFourtain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
public:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *Body;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *Water;
	UPROPERTY(VisibleAnywhere)
	UPointLightComponent *Light;
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent *Splash;

	//UPROPERTY(VisibleAnywhere)
	//URotatingMovementComponent *Movement;

	UPROPERTY(EditAnywhere, Category=ID)
	int32 ID;

	
private:
	UPROPERTY(EditAnywhere, Category=Stat, Meta = (AllowPrivateAccess = true))
	float RotateSpeed = 30.f;

	CEchoerver echoServer;
	bool bSend = false;
};
