// Fill out your copyright notice in the Description page of Project Settings.

#include "Fourtain.h"



// Sets default values
AFourtain::AFourtain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "BODY" ) );
	Water = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "WATER" ) );
	Light = CreateDefaultSubobject<UPointLightComponent>( TEXT( "LIGHT" ) );
	Splash = CreateDefaultSubobject<UParticleSystemComponent>( TEXT( "SPLASH" ) );
	//Movement = CreateDefaultSubobject<URotatingMovementComponent>( TEXT( "MOVEMENT" ) );

	RootComponent = Body;
	Water->SetupAttachment( Body );
	Light->SetupAttachment( Body );
	Splash->SetupAttachment( Body );

	Water->SetRelativeLocation( FVector( 0.f, 0.f, 135.f ) );
	Light->SetRelativeLocation( FVector( 0.f, 0.f, 195.f ) );
	Splash->SetRelativeLocation( FVector( 0.f, 0.f, 195.f ) );

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BODY( TEXT( "StaticMesh'/Game/InfinityBladeGrassLands/Environments/Plains/Env_Plains_Ruins/StaticMesh/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01'" ) );
	if( SM_BODY.Succeeded() )
	{
		Body->SetStaticMesh( SM_BODY.Object );
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_WATER( TEXT( "StaticMesh'/Game/InfinityBladeGrassLands/Effects/FX_Meshes/Env/SM_Plains_Fountain_02.SM_Plains_Fountain_02'" ) );
	if( SM_WATER.Succeeded() )
	{
		Water->SetStaticMesh( SM_WATER.Object );
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_SPLASH( TEXT( "ParticleSystem'/Game/InfinityBladeGrassLands/Effects/FX_Ambient/Water/P_Water_Fountain_Splash_Base_01.P_Water_Fountain_Splash_Base_01'" ) );
	if( PS_SPLASH.Succeeded() )
	{
		Splash->SetTemplate( PS_SPLASH.Object );
	}

	//Movement->RotationRate = FRotator( 0.f, RotateSpeed, 0.f );
	
	ABLOG_S( Warning );
}

AFourtain::~AFourtain()
{
	ABLOG_S( Warning );
}

// Called when the game starts or when spawned
void AFourtain::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG( ArenaBattle, Warning, TEXT( "Actor Name : %s, ID : %d, Location X : %.3f" ), *GetName(), ID, GetActorLocation().X );

	echoServer.Connect( TEXT("192.168.0.40"), 21010 );
}

void AFourtain::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	Super::EndPlay( EndPlayReason );

	ABLOG_S(Warning);
}

void AFourtain::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_S( Warning );
}

// Called every frame
void AFourtain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation( FRotator( 0.f, RotateSpeed * DeltaTime, 0.f ) );

	echoServer.OnTick();

	if( echoServer.IsConnected() && bSend == false )
	{
		bSend = true;

		echoServer.m_proxy.LogInReq( HostID_None, RmiContext::Reliable, TEXT("test"), FDateTime.Now(), TArray<int>() );
	}
}

