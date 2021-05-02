using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class TimerController : MonoBehaviour
{
    public static TimerController instance;
    public TMP_Text timer;

    private TimeSpan timePlaying;
    private bool isTimerPlaying;

    private float elapsedTime;

    private void Awake() 
    {
        instance = this;
        //timer = GetComponent<TMP_Text>();
    }

    // Start is called before the first frame update
    void Start()
    {
        timer.text = "Time: 00:00.00";
        isTimerPlaying = false;
    }

    // Update is called once per frame
    void Update()
    {
        if (elapsedTime >= 60.0f) 
        {
            EndTimer();
        }
    }

    public void BeginTimer() 
    {
        isTimerPlaying = true;
        elapsedTime = 0f;

        StartCoroutine(UpdateTimer());
    }

    public void EndTimer() 
    {
        isTimerPlaying = false;
    }

    private IEnumerator UpdateTimer() 
    {
        while (isTimerPlaying) 
        {
            elapsedTime += Time.deltaTime;
            timePlaying = TimeSpan.FromSeconds(elapsedTime);
            string timePlayingStr = "Time: " + timePlaying.ToString("mm':'ss'.'ff");
            timer.text = timePlayingStr;
            yield return null;
        }
        yield return null;
    }
}
