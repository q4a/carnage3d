#include "stdafx.h"
#include "Pedestrian.h"

PedestrianControl::PedestrianControl(Pedestrian& pedestrian)
    : mPedestrian(pedestrian)
{
    ResetControl();
}

void PedestrianControl::ResetControl()
{
    mTurnAngle = 0.0f;
    mTurnLeft = false;
    mTurnRight = false;
    mWalkBackward = false;
    mWalkForward = false;
    mRunForward = false;
}

void PedestrianControl::SetTurnLeft(bool turnEnabled)
{
    mTurnLeft = turnEnabled;
    mTurnAngle = 0.0f;
}

void PedestrianControl::SetTurnRight(bool turnEnabled)
{
    mTurnRight = turnEnabled;
    mTurnAngle = 0.0f;
}

void PedestrianControl::SetTurnAngle(float turnAngle)
{
    mTurnAngle = turnAngle;
    mTurnLeft = false;
    mTurnRight = false;
}

void PedestrianControl::SetWalkForward(bool walkEnabled)
{
    mWalkForward = walkEnabled;
}

void PedestrianControl::SetWalkBackward(bool walkEnabled)
{
    mWalkBackward = walkEnabled;
}

bool PedestrianControl::IsTurnAround() const
{
    return mTurnLeft || mTurnRight || fabs(mTurnAngle) > 0.01f;
}

bool PedestrianControl::IsMoves() const
{
    return mWalkBackward || mWalkForward || mRunForward;
}

void PedestrianControl::SetRunForward(bool runEnabled)
{
    mRunForward = runEnabled;
}

//////////////////////////////////////////////////////////////////////////

Pedestrian::Pedestrian()
    : mPosition()
    , mPrevPosition()
    , mHeading()
    , mPrevHeading()
    , mSphereRadius(1.0f)
    , mDead()
    , mVelocity()
    , mCurrentAnimID(eSpriteAnimationID_Null)
    , mControl(*this)
{
}

void Pedestrian::EnterTheGame()
{
    mDead = false;
    mLiveTicks = 0;
    mVelocity = {0.0f, 0.0f, 0.0f};
    mCurrentAnimID = eSpriteAnimationID_Null;
    // set initial state and animation
    SwitchToAnimation(eSpriteAnimationID_Ped_StandingStill, eSpriteAnimLoop_FromStart);
}

void Pedestrian::UpdateFrame(Timespan deltaTime)
{
    mAnimation.UpdateFrame(deltaTime);

    mLiveTicks += deltaTime;

    // try to turn around
    if (mControl.IsTurnAround())
    {
        float anglePerFrame = 0.0f;
        if (mControl.mTurnLeft || mControl.mTurnRight)
        {
            anglePerFrame = (mControl.mTurnLeft ? -1.0f : 1.0f) * gGameRules.mPedestrianTurnSpeed * deltaTime.ToSeconds();
        }
        else // specific angle
        {
            anglePerFrame = (mControl.mTurnAngle * gGameRules.mPedestrianTurnSpeed * deltaTime.ToSeconds());
            mControl.mTurnAngle -= anglePerFrame;
        }
        mHeading = cxx::normalize_angle_180(mHeading + anglePerFrame);
    }
    // try walk
    if (mControl.IsMoves())
    {
        float moveSpeed = 0.0f;
        if (mControl.mRunForward)
        {
            moveSpeed = gGameRules.mPedestrianRunSpeed;
        }
        else if (mControl.mWalkForward)
        {
            moveSpeed = gGameRules.mPedestrianWalkSpeed;
        }
        else if (mControl.mWalkBackward)
        {
            moveSpeed = -gGameRules.mPedestrianBackWalkSpeed;
        }
        // get current direction
        glm::vec2 signVector;
        cxx::vector_from_angle(glm::radians(mHeading + 90.0f), signVector);

        glm::vec2 walkDistance = signVector * moveSpeed * deltaTime.ToSeconds();
        mPosition.x += walkDistance.x;
        mPosition.y += walkDistance.y;
    }
}

void Pedestrian::SwitchToAnimation(eSpriteAnimationID animation, eSpriteAnimLoop loopMode)
{
    if (mCurrentAnimID != animation)
    {
        mAnimation.SetNull();
        if (!gCarnageGame.mCityScape.mStyleData.GetSpriteAnimation(animation, mAnimation.mAnimData)) // todo
        {
            debug_assert(false);
        }
        mCurrentAnimID = animation;
    }
    mAnimation.PlayAnimation(loopMode);
}

void Pedestrian::SetHeading(float rotationDegrees)
{
    mHeading = rotationDegrees;
    mPrevHeading = rotationDegrees;
}

void Pedestrian::SetPosition(float posx, float posy, float posz)
{
    mPosition.x = posx;
    mPosition.y = posy;
    mPosition.z = posz;
    mPrevPosition = mPosition;
}

//////////////////////////////////////////////////////////////////////////

bool PedestrianManager::Initialize()
{
    return true;
}

void PedestrianManager::Deinit()
{
    for (Pedestrian* currPedestrian: mActivePedsList)
    {
        if (currPedestrian)
        {
            mPedsPool.destroy(currPedestrian);
        }
    }
    for (Pedestrian* currPedestrian: mDestroyPedsList)
    {
        if (currPedestrian)
        {
            mPedsPool.destroy(currPedestrian);
        }
    }
}

void PedestrianManager::UpdateFrame(Timespan deltaTime)
{
    DestroyPendingPeds();
    
    for (Pedestrian* currentPed: mActivePedsList) // warning: dont add new peds during this loop
    {
        if (currentPed == nullptr)
            continue;

        currentPed->UpdateFrame(deltaTime);
    }

    RemoveOffscreenPeds();

    // update physics
}

void PedestrianManager::DestroyPendingPeds()
{
    for (Pedestrian* currentPed: mDestroyPedsList)
    {
        if (currentPed == nullptr)
            continue;

        mPedsPool.destroy(currentPed);
    }
    mDestroyPedsList.clear();
}

void PedestrianManager::RemovePedestrian(Pedestrian* pedestrian)
{
    debug_assert(pedestrian);
    if (pedestrian == nullptr)
        return;

    auto found_iter = std::find(mActivePedsList.begin(), mActivePedsList.end(), pedestrian);
    if (found_iter != mActivePedsList.end())
    {
        *found_iter = nullptr;
    }

    found_iter = std::find(mDestroyPedsList.begin(), mDestroyPedsList.end(), pedestrian);
    if (found_iter != mDestroyPedsList.end())
    {
        debug_assert(false);
        return;
    }

    mDestroyPedsList.push_back(pedestrian);
}

void PedestrianManager::RemoveOffscreenPeds()
{
}

Pedestrian* PedestrianManager::CreateRandomPed(const glm::vec3& position)
{
    Pedestrian* instance = mPedsPool.create();
    debug_assert(instance);

    AddToActiveList(instance);

    // init
    instance->mPosition = position;
    instance->mPrevPosition = position;
    instance->EnterTheGame();
    return instance;
}

void PedestrianManager::AddToActiveList(Pedestrian* ped)
{
    for (Pedestrian*& curr: mActivePedsList)
    {
        if (curr == nullptr)
        {
            curr = ped;
            return;
        }
    }
    mActivePedsList.push_back(ped);
}
