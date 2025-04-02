
class CheckStaticDefense : public Leaf
{
public:
    CheckStaticDefense(Enemy* anEnemy) : myEnemy(anEnemy) {}

    Status Update() override
    {
        if (myEnemy->IsDefensePointAttackable())
        {
            return Status::Success;
        }
        return Status::Failure;
    }
private:
    Enemy* myEnemy;
};

class CheckPlayerVisible : public Leaf
{
public:
    CheckPlayerVisible(Enemy* anEnemy) : myEnemy(anEnemy) {}

    Status Update() override
    {
        if (myEnemy->RaycastTowardsPlayer())
        {
            myEnemy->GetShouldSeekTimer().Reset();
            return Status::Success;
        }
        return Status::Failure;
    }
private:
    Enemy* myEnemy;
};

class CheckPlayerSearch : public Leaf
{
public:
    CheckPlayerSearch(Enemy* anEnemy) : myEnemy(anEnemy) {}

    Status Update() override
    {
        if (myEnemy->GetShouldSeekPlayer())
        {
            myEnemy->SearchLastSeenPlayer();
            return Status::Success;
        }
        return Status::Failure;
    }

private:
    Enemy* myEnemy;
};

class FollowPath : public Leaf
{
public:
    FollowPath(Enemy* anEnemy) : myEnemy(anEnemy) {}

    Status Update() override
    {
        myEnemy->FollowNavmeshPath();
        return Status::Success;
    }
private:
    Enemy* myEnemy;
};

class AttackAction : public Leaf
{
public:
    AttackAction(Enemy* anEnemy) : myEnemy(anEnemy) {}

    Status Update() override
    {
        if (myEnemy->GetEnemyType() == eEnemyType::FlyingShooting)
        {
            if (myEnemy->GetShouldAttackPlayer())
            {
                myEnemy->AttackAction();
                return Status::Success;
            }
            else if (myEnemy->GetShouldAttackDefensePoint())
            {
                myEnemy->AttackAction();
                return Status::Success;
            }
        }

        return Status::Failure;
    }
private:
    Enemy* myEnemy;
};

class EnemyBehavior
{
public:
    EnemyBehavior() { myBehaviorTree = nullptr;  myEnemy = nullptr; }
    ~EnemyBehavior() { myBehaviorTree = nullptr;  myEnemy = nullptr; }

    EnemyBehavior(Enemy* aEnemy) : myEnemy(aEnemy)
    {
        BuildTree();
    }

    void Update()
    {
        myBehaviorTree->Update();
    }

private:
    void BuildTree()
    {
        // [Root Selector]
        //    |->[Offensive Sequence]
        //    |      |-> [Target Selection Selector]
        //    |      |      |-> CheckPlayerVisible
        //    |      |      |-> CheckStaticDefense
        //    |      |-> AttackAction (Based on enemy type)
        //    |-> Follow Navmesh
        Builder builder;
        builder.Composites<Selector>()
            // Offensive branch: try to attack.
            .Composites<Sequence>()
            // Selector that checks if either player is visible or static defense condition is met.
            .Composites<Selector>()
            .Leaf<CheckPlayerVisible>(myEnemy)
            .Leaf<CheckPlayerSearch>(myEnemy) // Not used by ground enemy
            .Leaf<CheckStaticDefense>(myEnemy)
            .End()
            // Only if one of the above succeeds, run the attack action.
            .Leaf<AttackAction>(myEnemy)
            .End()
            // Fallback branch: if offensive branch fails, follow the navmesh path.
            .Leaf<FollowPath>(myEnemy)
            .End();

        myBehaviorTree = std::make_shared<BehaviourTree>(builder.Build());
        assert(myBehaviorTree != nullptr && "Failed to build behavior tree!");
    }

    std::shared_ptr<BehaviourTree> myBehaviorTree;
    Enemy* myEnemy;
};