#ifndef FUNC_ARSENIO_PORTAL_H
#define FUNC_ARSENIO_PORTAL_H

#include "cbase.h"
#include "triggers.h"
#include "entityoutput.h"
#include "util.h"
#include "materialsystem/imesh.h"
#include "info_camera_link.h"

#define PORTAL_CAMERA_FOV 90.0f 

class CFuncArsenioPortal : public CBaseTrigger
{
public:
    DECLARE_CLASS(CFuncArsenioPortal, CBaseTrigger);
    DECLARE_DATADESC();

    CFuncArsenioPortal();

    void Spawn() override;
    void Think() override;
    void Precache() override;

    void ActivatePortal();
    void DeactivatePortal();
    void LinkPortals();

    void TeleportEntity(CBaseEntity* pEntity);


    void SetLinkedPortal(CFuncArsenioPortal* pLinkedPortal) { m_hLinkedPortal = pLinkedPortal; }

    void InputLinkPortal(inputdata_t& inputdata);

    bool IsEntityInPortal(CBaseEntity* pEntity);

    void UpdatePortalView();

    CFuncArsenioPortal* GetLinkedPortal() { return dynamic_cast<CFuncArsenioPortal*>(m_hLinkedPortal.Get()); }

    float m_flLastTeleportTime;

private:
    void CreateCamera();
    void CreateMonitor();

    EHANDLE m_hLinkedPortal;
    CHandle<CBaseEntity> m_pPortalCamera;
    CHandle<CBaseEntity> m_pPortalMonitor;


    COutputEvent m_OnPortalActivated;
    COutputEvent m_OnPortalDeactivated;

    bool m_bPortalActive;

};

#endif 
