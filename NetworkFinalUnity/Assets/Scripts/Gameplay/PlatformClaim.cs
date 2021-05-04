using MLAPI;
//using MLAPI.Connection;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class PlatformClaim : MonoBehaviour
{
    private PlayerScript pOwner;
    //private ulong pOwner;
    public int platformIndex;
    Color playerClaim;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void SetOwner(PlayerScript playerHit) 
    {
        Debug.Log("SetOwnerCalled");
        if (pOwner != null) 
        {
            pOwner.playerScore -= 1;
            pOwner.UpdateScoreText();
        }
        pOwner = playerHit;
        playerHit.playerScore += 1;
        playerHit.UpdateScoreText();
        playerClaim = playerHit.generatedColor;
        var tileRenderer = this.GetComponent<Renderer>();
        tileRenderer.material.SetColor("_Color", playerClaim);

        //elapsedTime += Time.deltaTime;
        //timePlaying = TimeSpan.FromSeconds(elapsedTime);
        //string timePlayingStr = "Time: " + timePlaying.ToString("mm':'ss'.'ff");
        //timer.text = timePlayingStr;
        //tileRenderer.material.SetColor("_Color", Color.black);
        /*Debug.Log("SetOwnerCalled");
        pOwner = playerHit.playerID;
        playerClaim = playerHit.generatedColor;
        var tileRenderer = this.GetComponent<Renderer>();
        tileRenderer.material.SetColor("_Color", playerClaim);*/
    }

    private void OnCollisionEnter(Collision collision) 
    {
        if (!(NetworkManager.Singleton.IsServer)) 
        {
            return;
        }
            //use ulong owner to specify when object is owned already in final version
        PlayerScript hit = collision.collider.GetComponent<PlayerScript>();


        if (pOwner != hit)
        {
            SetOwner(hit);
            NetworkInterface.Instance.BroadcastMapEvent(hit.playerID, platformIndex);
            Debug.Log("Unowned Collidision");
            
        }
        else 
        {
            Debug.Log("Owned Collision");
        }

        /*if (pOwner == hit.playerID)
        {
            Debug.Log("Owned Collision");
        }
        else
        {
            SetOwner(hit);
            NetworkInterface.Instance.BroadcastMapEvent(hit.playerID, platformIndex);
            Debug.Log("Unowned Collidision");
        }*/

    }
}
