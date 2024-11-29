#include "cbase.h"
#include "func_arsenio_portal.h"
#include "props.h"
#include "eventqueue.h"
#include "utlvector.h"
#include "debugoverlay_shared.h"

LINK_ENTITY_TO_CLASS(func_arsenio_portal, CFuncArsenioPortal);

BEGIN_DATADESC(CFuncArsenioPortal)
DEFINE_KEYFIELD(m_hLinkedPortal, FIELD_EHANDLE, "linkedportal"),
DEFINE_FIELD(m_bPortalActive, FIELD_BOOLEAN),
DEFINE_OUTPUT(m_OnPortalActivated, "OnPortalActivated"),
DEFINE_OUTPUT(m_OnPortalDeactivated, "OnPortalDeactivated"),
DEFINE_INPUTFUNC(FIELD_EHANDLE, "LinkPortal", InputLinkPortal),
END_DATADESC()

CFuncArsenioPortal::CFuncArsenioPortal()
{
    m_hLinkedPortal = NULL;
    m_bPortalActive = false;
    m_pPortalCamera = NULL;
    m_pPortalMonitor = NULL;

    m_flLastTeleportTime = 0.0f;

    DevMsg("CFuncArsenioPortal: Constructor called.\n");
}

void CFuncArsenioPortal::Precache()
{

    PrecacheScriptSound("Portal.Teleport");
    DevMsg("CFuncArsenioPortal: Resources precached.\n");
}

void CFuncArsenioPortal::Spawn()
{

    SetSolid(SOLID_BSP);
    SetMoveType(MOVETYPE_NONE);
    SetModelName(NULL_STRING);

    DevMsg("CFuncArsenioPortal: Spawned with no model, brush entity.\n");


    LinkPortals();


    SetNextThink(gpGlobals->curtime + 0.1f);
    DevMsg("CFuncArsenioPortal: Think function set.\n");
}

void CFuncArsenioPortal::LinkPortals()
{

    if (m_hLinkedPortal)
    {
        DevMsg("CFuncArsenioPortal: Successfully linked to portal: %s\n", m_hLinkedPortal->GetEntityName());

        ActivatePortal();
    }
    else
    {
        DevMsg("CFuncArsenioPortal: No linked portal, deactivating portal.\n");

        DeactivatePortal();
    }
}

void CFuncArsenioPortal::InputLinkPortal(inputdata_t& inputdata)
{

    CBaseEntity* pEntity = inputdata.value.Entity().Get();
    CFuncArsenioPortal* pLinkedPortal = dynamic_cast<CFuncArsenioPortal*>(pEntity);

    if (pLinkedPortal)
    {
        DevMsg("CFuncArsenioPortal: Linking to portal: %s\n", pLinkedPortal->GetEntityName());
        m_hLinkedPortal = pLinkedPortal;
        LinkPortals();
    }
    else
    {
        DevMsg("CFuncArsenioPortal: Invalid portal for linking.\n");
    }
}


void CFuncArsenioPortal::ActivatePortal()
{
    if (m_bPortalActive)
    {
        DevMsg("CFuncArsenioPortal: Portal already active.\n");
        return;
    }


    m_bPortalActive = true;
    UpdatePortalView();
    m_OnPortalActivated.FireOutput(this, this);

    DevMsg("CFuncArsenioPortal: Portal activated.\n");
}

void CFuncArsenioPortal::DeactivatePortal()
{
    if (!m_bPortalActive)
    {
        DevMsg("CFuncArsenioPortal: Portal already deactivated.\n");
        return;
    }


    m_bPortalActive = false;
    if (m_pPortalCamera)
    {
        UTIL_Remove(m_pPortalCamera);
        DevMsg("CFuncArsenioPortal: Camera removed.\n");
    }

    if (m_pPortalMonitor)
    {
        UTIL_Remove(m_pPortalMonitor);
        DevMsg("CFuncArsenioPortal: Monitor removed.\n");
    }

    m_OnPortalDeactivated.FireOutput(this, this);
    DevMsg("CFuncArsenioPortal: Portal deactivated.\n");
}

void CFuncArsenioPortal::Think()
{

    for (int i = 0; i < MAX_EDICTS; ++i)
    {
        CBaseEntity* pEntity = UTIL_EntityByIndex(i);
        if (!pEntity || !pEntity->IsPlayer())
            continue;


        if (m_bPortalActive && IsEntityInPortal(pEntity) && gpGlobals->curtime - m_flLastTeleportTime > 0.5f)
        {
            DevMsg("CFuncArsenioPortal: Player %s is inside the portal, teleporting.\n", pEntity->GetClassname());
            TeleportEntity(pEntity);
            m_flLastTeleportTime = gpGlobals->curtime;
        }
    }

    SetNextThink(gpGlobals->curtime + 0.1f);
}


bool CFuncArsenioPortal::IsEntityInPortal(CBaseEntity* pEntity)
{

    bool inPortal = (pEntity->GetAbsOrigin().DistTo(GetAbsOrigin()) < 50.0f);
    if (inPortal)
    {
        DevMsg("CFuncArsenioPortal: Entity %s is within portal radius.\n", pEntity->GetClassname());
    }
    else
    {
        DevMsg("CFuncArsenioPortal: Entity %s is outside portal radius.\n", pEntity->GetClassname());
    }
    return inPortal;
}

void CFuncArsenioPortal::TeleportEntity(CBaseEntity* pEntity)
{

    if (m_hLinkedPortal)
    {
        pEntity->SetAbsOrigin(m_hLinkedPortal->GetAbsOrigin());
        pEntity->SnapToFloor();


        EmitSound("Portal.Teleport");
        DevMsg("CFuncArsenioPortal: Teleported entity %s to linked portal.\n", pEntity->GetClassname());
    }
}

void CFuncArsenioPortal::CreateCamera()
{

    m_pPortalCamera = CreateEntityByName("point_camera");
    if (m_pPortalCamera)
    {

        DevMsg("CFuncArsenioPortal: Camera entity created.\n");
    }
    else
    {
        DevMsg("CFuncArsenioPortal: Failed to create camera entity.\n");
    }
}

void CFuncArsenioPortal::CreateMonitor()
{

    m_pPortalMonitor = CreateEntityByName("func_monitor");
    if (m_pPortalMonitor)
    {



        m_pPortalMonitor->KeyValue("camera", m_hLinkedPortal->GetEntityName().ToCStr());
        m_pPortalMonitor->SetAbsOrigin(GetAbsOrigin());
        m_pPortalMonitor->SetAbsAngles(GetAbsAngles());
        m_pPortalMonitor->Spawn();

        DevMsg("CFuncArsenioPortal: Monitor created.\n");
    }
    else
    {
        DevMsg("CFuncArsenioPortal: Failed to create monitor entity.\n");
    }
}

void ArsenioRefreshPortalCameras()
{

    for (int i = 0; i < MAX_EDICTS; ++i)
    {
        CBaseEntity* pEntity = UTIL_EntityByIndex(i);
        if (pEntity && FStrEq(pEntity->GetClassname(), "func_arsenio_portal"))
        {
            CFuncArsenioPortal* pPortal = static_cast<CFuncArsenioPortal*>(pEntity);
            pPortal->UpdatePortalView();
            DevMsg("ArsenioRefreshPortalCameras: Refreshed portal cameras for %s\n", pPortal->GetEntityName());
        }
    }
}


ConCommand arsenio_refresh_portal_cameras("arsenio_refresh_portal_cameras", ArsenioRefreshPortalCameras, "Refresh all portal cameras", FCVAR_NONE);

void CFuncArsenioPortal::UpdatePortalView()
{

    if (!m_pPortalCamera)
    {
        CreateCamera();
    }


    if (!m_pPortalMonitor)
    {
        CreateMonitor();
    }


    if (m_pPortalCamera)
    {
        m_pPortalCamera->SetAbsOrigin(GetAbsOrigin());
        m_pPortalCamera->SetAbsAngles(GetAbsAngles());
        DevMsg("CFuncArsenioPortal: Camera updated to portal position.\n");
    }


    if (m_pPortalMonitor)
    {
        m_pPortalMonitor->SetAbsOrigin(GetAbsOrigin());
        m_pPortalMonitor->SetAbsAngles(GetAbsAngles());
        DevMsg("CFuncArsenioPortal: Monitor updated to show linked portal view.\n");
    }
}
